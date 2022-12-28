#include "stdafx.h"
#include "Client.h"
#include <WS2tcpip.h>


Client::Client()
{
}

Client::~Client()
{
	Running = false;
	RecieveThread.join();
	closesocket(ClientSocket);
	WSACleanup();
}

bool Client::CreateClientSocket()
{
	WSAData WsaData;
	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0)
	{
		Log::PrintLog("Fail to Init WinSock");
		return false;
	}

	ClientSocket = WSASocket(AF_INET, SOCK_STREAM, NULL, NULL, 0, WSA_FLAG_OVERLAPPED);
	//ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ClientSocket == INVALID_SOCKET)
	{
		Log::PrintLog("Fail to Create Client Socket");
		return false;
	}

	int reUseAddr = 1;
	::setsockopt(ClientSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reUseAddr, (int)sizeof(reUseAddr));
	ZeroMemory(IODatas[Read].Data, BUF_SIZE);
	return true;
}

bool Client::ConnectToServer()
{
	SOCKADDR_IN ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons((USHORT)13000);
	inet_pton(AF_INET, "127.0.0.1", &(ServerAddr.sin_addr));
	int result = WSAConnect(ClientSocket, (sockaddr*)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);
	if (result == SOCKET_ERROR)
	{
		Log::PrintLog("Fail to Connect");
		return false;
	}
	Running = true;
	RecieveThread = thread(&Client::RecieveWork, this);
	SetHandler();
	return true;
}

void Client::SendToServer(char* data)
{
	WSABUF Buffer = {0,};
	DWORD flags = 0;
	//DWORD recvBytes;
	ST_IOHeader* Header = (ST_IOHeader*)data;
	//Buffer.len = Header->Size;
	//Buffer.buf = data;
	//WSASend(ClientSocket, &Buffer, 1, &recvBytes, flags, &IODatas[Read].Overlapped, NULL);
	send(ClientSocket, data, Header->Size, flags);
}

void Client::SendUserInfo()
{
	IODatas[Write].Usage = E_IOUsage::Write;
	ST_UserInfo Info;
	char Name[16] = "YuChoongHo";
	Info.SetInfo(Name,Name);
	WSABUF Buffer = { 0 };
	DWORD flags = 0;
	DWORD recvBytes;
	Buffer.len = BUF_SIZE;
	Buffer.buf = (char*)&Info;
	WSASend(ClientSocket, &Buffer, 1, &recvBytes, flags, &IODatas[Write].Overlapped, NULL);
}

void Client::RecieveWork()
{
	while (Running)
	{
		IODatas[Read].Usage = E_IOUsage::Read;
		WSABUF Buffer = {0,};
		int flags = 0;
		Buffer.buf = IODatas[Read].Data + IODatas[Read].CurBytes;
		Buffer.len = BUF_SIZE - IODatas[Read].CurBytes;
		int result = recv(ClientSocket, Buffer.buf, Buffer.len, flags);

		IODatas[Read].CurBytes += result;
	
		while (IODatas[Read].DataPointer < IODatas[Read].CurBytes)
		{
			if ((IODatas[Read].CurBytes - IODatas[Read].DataPointer) < sizeof(ST_IOHeader))
				break;
			
			ST_IOHeader* Header = (ST_IOHeader*)(IODatas[Read].Data + IODatas[Read].DataPointer);
			
			if ((IODatas[Read].CurBytes - IODatas[Read].DataPointer) < Header->Size)
				break;

			if (Handler[Header->IOType])
			{
				//ST_HandleData Data{ Header->IOType, Header->Size };
				//memcpy(Data.data, IODatas[Read].Data + IODatas[Read].DataPointer, Header->Size);
				//std::async(&Client::Process, this, Data);
				Handler[Header->IOType](IODatas[Read].Data + IODatas[Read].DataPointer, Header->Size);
			}
			else
				break;

		}
		if (IODatas[Read].CurBytes == BUF_SIZE)
		{
			lock_guard<mutex> LockGuard(ReadLock);
			memcpy(IODatas[Read].Data, IODatas[Read].Data + IODatas[Read].DataPointer, BUF_SIZE - IODatas[Read].DataPointer);
			IODatas[Read].CurBytes = BUF_SIZE - IODatas[Read].DataPointer;
			IODatas[Read].DataPointer = 0;
		}
		else
		{
			lock_guard<mutex> LockGuard(ReadLock);
			IODatas[Read].CurBytes = 0;
			IODatas[Read].DataPointer = 0;
		}
	}
}

void Client::Process(ST_HandleData data)
{
	Handler[data.type](data.data, data.size);
}


