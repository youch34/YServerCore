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

	for (int i = 0; i < 5; i++)
	{
		ProcessThreads.push_back(thread(&Client::Process, this));
	}

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
		Buffer.buf = IODatas[Read].Data;
		Buffer.len = BUF_SIZE;
		int result = recv(ClientSocket, Buffer.buf, Buffer.len, flags);
		Log::PrintLog("%d", result);
		InputWork(result);
		ZeroMemory(&IODatas[Read], sizeof(ST_IOData));
	}
}

void Client::PacketWork()
{
}

void Client::InputWork(int datasize)
{
	char* DataPointer = IODatas[Read].Data;
	int ReadSize = 0;
	while(ReadSize < datasize)
	{
		ST_IOHeader* Header = (ST_IOHeader*)DataPointer;
		if (Header <= 0)
			return;
		shared_ptr<YPacket> Packet(static_cast<YPacket*>(malloc(Header->Size)), free);
		memcpy(Packet.get(), DataPointer, Header->Size);
		WorkQueue.push(Packet);
		DataPointer += Header->Size;
		ReadSize += Header->Size;
		ProcessCondition.notify_one();
	}
}

void Client::Process()
{
	while (Running) 
	{
		unique_lock<mutex> ULock(MsgLock);
		ProcessCondition.wait(ULock, [this] { return !WorkQueue.empty(); });
		//[this] { return !WorkQueue.empty(); }
		ST_ChatMessage* msg = (ST_ChatMessage*)WorkQueue.front().get();
		WorkQueue.pop();
	}
}


