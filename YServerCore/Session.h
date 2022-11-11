#pragma once

#include <WinSock2.h>
#include "PacketProcess.h"

struct ST_SocketData 
{
	SOCKET Socket;
	SOCKADDR_IN SockAddr;
};

enum E_IOUsage
{
	Read = 0,
	Write = 1,
	Error = 2
};

enum E_SessionState
{
	Wait = 0,
	Connect,
	Login
};

struct ST_IOData
{
public:
	OVERLAPPED Overlapped;
	E_IOUsage Usage;
	size_t TotalBytes;
	size_t CurBytes;
	char Data[BUF_SIZE];
};

class Session
{
public:
	Session();
	~Session();
	void SetIODataUsage()
	{
		IODatas[Read].Usage = E_IOUsage::Read;
		IODatas[Write].Usage = E_IOUsage::Write;
	}

	void OnAccept(ST_SocketData NewSocketData);

	void OnRecv(size_t TransferSize);
	void ReadyToRecieve();
	void Recv(WSABUF Buffer);

	void OnSend(size_t TransferSize);
	void SendPacket(char* Packet);
	void Send(WSABUF Buffer);

	void SetUserInfo(ST_UserInfo userinfo) { UserInfo = userinfo; }
	void SetSessionState(E_SessionState state) { SessionState = state; }


public:
	ST_SocketData SocketData;
	PacketProcess PacketProcessor;
	ST_IOData IODatas[3] = {0,};
	int SessionID = 0;

	ST_UserInfo UserInfo;

	E_SessionState SessionState = E_SessionState::Wait;
};