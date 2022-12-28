#pragma once
#include "stdafx.h"
#include "Session.h"
class SessionManager 
{
private:
	SessionManager() 
	{ 
		Sessions.reserve(sizeof(Session) * 1000); 
		Running = true;
		//HeartBeatThread = thread(&SessionManager::HeartBeat, this);
	};
	~SessionManager() 
	{
		Running = false;
		HeartBeatThread.join();
		for (auto& Session : Sessions) 
		{ delete Session; } 
	};
public:
	static SessionManager& Get() 
	{
		static SessionManager Instance;
		return Instance;
	}

public:
	Lock SessionLock;
	void AddSession(Session* Session) 
	{ 
		lock_guard<shared_mutex> S_Lock(S_Mutex);	
		Session->SessionID = SessionCount;  
		Sessions.push_back(Session); SessionCount++; 
	}
	void CloseSession(Session* Session)
	{
		lock_guard<shared_mutex> S_Lock(S_Mutex);
		Sessions[Session->SessionID] = Sessions.back();
		Sessions.pop_back();
		SessionCount--;
		delete Session;
	}
	void AllMessage(char* Msg) 
	{
		shared_lock<shared_mutex> S_Lock(S_Mutex);
		for (auto& Session : Sessions)
		{
			Session->SendPacket(Msg);
		}
	}
	void HeartBeat() 
	{
		while (Running)
		{
			Sleep(60000);
			for (auto& Session : Sessions)
			{
				ST_IOHeader Header;
				Session->SendPacket((char*)&Header);
			}
		}
	}
private:
	vector<Session*> Sessions;
	int SessionCount = 0;
	mutable shared_mutex S_Mutex;
	thread HeartBeatThread;
	bool Running = false;
};