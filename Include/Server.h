#pragma once

#include <WinSock2.h>
#include "Session.h"
#include "Thread.h"
#include "SessionManager.h"
class Server 
{
public:
	Server();
	~Server();

public:
	bool Run();
	bool CreateListenSocket();
	bool OnAccept(ST_SocketData SocketData);
	void AcceptWork(Server* ServerPtr);
public:
	Thread ThreadManager;
	SOCKET ListenSocket = INVALID_SOCKET;
	HANDLE Iocp = NULL;
	thread AcceptThread;
	thread NetworkProcessThread[4];
	list<Session*> Sessions;
private:

};