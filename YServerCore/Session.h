#pragma once

#include <WinSock2.h>


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
	OVERLAPPED Overlapped = {0,};
	E_IOUsage Usage = E_IOUsage::Error;
	size_t TotalBytes = NULL;
	size_t CurBytes = NULL;
	size_t DataPointer = NULL;
	atomic<bool> Complete;
	char Data[BUF_SIZE] = {0,};
};

class Session
{
public:
	Session();
	~Session();
	void SetIODataUsage()
	{
		ZeroMemory(&IODatas[Read], sizeof(ST_IOData));
		ZeroMemory(&IODatas[Write], sizeof(ST_IOData));
		IODatas[Read].Usage = E_IOUsage::Read;
		IODatas[Write].Usage = E_IOUsage::Write;
	}

	void OnAccept(ST_SocketData NewSocketData);

	void OnRecv(size_t TransferSize);
	void ReadyToRecieve();
	void Recv(WSABUF Buffer);

	void OnSend(size_t TransferSize);
	void SendPacket(char* Packet);
	void Send();

	void SetUserInfo(ST_UserInfo userinfo) { UserInfo = userinfo; }
	void SetSessionState(E_SessionState state) { SessionState = state; }


public:
	ST_SocketData SocketData;
	ST_IOData IODatas[3] = {0,};
	int SessionID = 0;
	ST_UserInfo UserInfo;
	Lock QueueLock;
	std::shared_mutex S_Mutex;
	E_SessionState SessionState = E_SessionState::Wait;
public:
	
};