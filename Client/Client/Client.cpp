#include "stdafx.h"
#include "Client.h"
#include <WS2tcpip.h>


Client::Client()
{
	ProcessThreads.reserve(5);
}

Client::~Client()
{
	RecieveThread.join();
	PopedReadData.clear();
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

	//for (int i = 0; i < 4; i++)
	//{
	//	ProcessThreads.push_back(thread(&Client::Process, this));
	//}
	SetHandler();
	return true;
}

void Client::SendToServer(char* Msg)
{
	ZeroMemory(&IODatas[Read], sizeof(ST_IOData));
	ST_ChatMessage Message;
	Message.Header.IOType = E_IOType::C_Chat;
	Message.SetMessage("DummyClient", Msg);
	WSABUF Buffer = {0,};
	DWORD flags = 0;
	DWORD recvBytes;
	Buffer.len = Message.Header.Size;
	Buffer.buf = (char*)&Message;
	WSASend(ClientSocket, &Buffer, 1, &recvBytes, flags, &IODatas[Read].Overlapped, NULL);
}

void Client::SendUserInfo()
{
	ZeroMemory(&IODatas[Write], sizeof(ST_IOData));
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
	while (true) 
	{
		IODatas[Read].Usage = E_IOUsage::Read;
		WSABUF Buffer = {0,};
		int flags = 0;
		Buffer.buf = IODatas[Read].Data + IODatas[Read].CurBytes;
		Buffer.len = BUF_SIZE - IODatas[Read].CurBytes;
		int result = recv(ClientSocket, Buffer.buf, Buffer.len, flags);
		//InputWork(result);
		if (result > 580)
		{
			cout << result << endl;
		}
		IODatas[Read].CurBytes += result;
		if (IODatas[Read].CurBytes == BUF_SIZE)
		{
			lock_guard<mutex> LockGuard(ReadLock);
			memcpy(IODatas[Read].Data, IODatas[Read].Data + IODatas[Read].DataPointer, BUF_SIZE - IODatas[Read].DataPointer);
			IODatas[Read].CurBytes = BUF_SIZE - IODatas[Read].DataPointer;
			IODatas[Read].DataPointer = 0;
		}

		while (IODatas[Read].DataPointer < IODatas[Read].CurBytes)
		{
			if ((IODatas[Read].CurBytes - IODatas[Read].DataPointer) < sizeof(ST_IOHeader))
				break;
			
			ST_IOHeader* Header = (ST_IOHeader*)(IODatas[Read].Data + IODatas[Read].DataPointer);
			
			if ((IODatas[Read].CurBytes - IODatas[Read].DataPointer) < Header->Size)
				break;

			if (Handler[Header->IOType])
			{
				Handler[Header->IOType](IODatas[Read].Data + IODatas[Read].DataPointer, Header->Size);
				//cout << ++count1 << endl;
				//HandleDataQueue.push(ST_HandleData{ Header->IOType, IODatas[Read].Data + IODatas[Read].DataPointer, Header->Size });
			}

		}
		if (IODatas[Read].DataPointer == IODatas[Read].CurBytes)
		{
			IODatas[Read].CurBytes = 0;
			IODatas[Read].DataPointer = 0;
		}
	}
}

void Client::Process()
{
	
}


