#pragma once

#include <WinSock2.h>


class Client 
{
public:
	Client();
	~Client();
	
	bool CreateClientSocket();
	bool ConnectToServer();

	void SendToServer(char* Msg);
	void SendUserInfo();
	void RecieveWork();

	void PacketWork();
	void InputWork(int datasize);
	void Process();
	void CleanPopedReadData();
public:
	SOCKET ClientSocket;
	thread RecieveThread;
	ST_IOData IODatas[3];
	list<char*> PopedReadData;
	queue<array<char, BUF_SIZE>> ReadDataQueue;
	queue<pair<char*, int>> WorkQueue;
	vector<thread> ProcessThreads;
	condition_variable ProcessCondition;
	Lock QueueLock;
	mutex MsgLock;
private:
	bool Running;
	int count1;
	int count2;
};