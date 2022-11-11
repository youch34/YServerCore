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
public:
	SOCKET ClientSocket;
	thread RecieveThread;
	ST_IOData IODatas[3];

private:

};