#pragma once
#include "stdafx.h"
#include "Session.h"
class SessionManager 
{
private:
	SessionManager() { Sessions.reserve(sizeof(Session)*1000); };
	~SessionManager() { for (auto& Session : Sessions) { delete Session; } };
public:
	static SessionManager& Get() 
	{
		static SessionManager Instance;
		return Instance;
	}

public:
	Lock SessionLock;
	void AddSession(Session* Session) { WriteLock(SessionLock);	Session->SessionID = SessionCount;  Sessions.push_back(Session); SessionCount++; }
	void CloseSession(Session* Session)
	{
		WriteLock(SessionLock);
		Sessions[Session->SessionID] = Sessions.back();
		Sessions.pop_back();
		SessionCount--;
		delete Session;
	}
	void AllMessage(char* Msg) 
	{
		WriteLock(SessionLock);
		for (auto& Session : Sessions)
		{
			Session->SendPacket(Msg);
		}
	}
private:
	vector<Session*> Sessions;
	int SessionCount = 0;
};