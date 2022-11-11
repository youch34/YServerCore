#include "stdafx.h"
#include "Client.h"
#include <WS2tcpip.h>

Client::Client()
{
	
}

Client::~Client()
{
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

	RecieveThread = thread(&Client::RecieveWork, this);

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
		ZeroMemory(&IODatas[Read], sizeof(ST_IOData));
		IODatas[Read].Usage = E_IOUsage::Read;
		WSABUF Buffer[BUF_SIZE] = {};
		int flags = 0;
		Buffer->buf = IODatas[Read].Data;
		Buffer->len = sizeof(ST_ChatMessage);
		int result = recv(ClientSocket, Buffer->buf, Buffer->len, flags);
		ST_ChatMessage* Msg = (ST_ChatMessage*)IODatas[Read].Data;
		Log::PrintLog(Msg->Message);
	}
}
