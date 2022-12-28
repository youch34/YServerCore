#include "stdafx.h"
#include "Server.h"
#include "Thread.h"
#include <WS2tcpip.h>

Server::Server()
{
}

Server::~Server()
{
	AcceptThread.join();
	closesocket(ListenSocket);
	for (auto Session : Sessions)
	{
		delete Session;
	}
	WSACleanup();
}

bool Server::Run()
{
	ThreadManager.SetServer(this);
	if (!ThreadManager.Initialize())
	{
		return false;
	}
	Iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, ThreadManager.MaxThreadCount);
	if (Iocp == nullptr)
	{
		Log::PrintLog("Failed to Create IOCP\n");
		return false;
	}

	if (!CreateListenSocket())
	{
		Log::PrintLog("Failed to Create ListenSocket\n");
		return false;
	}

	AcceptThread = thread(&Server::AcceptWork, this, this);
	return true;
}

bool Server::CreateListenSocket()
{
	WSADATA WsaData;
	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0)
	{
		Log::PrintLog("Failed to Init WindSock\n");
		return false;
	}

	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, NULL, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ListenSocket == INVALID_SOCKET)
	{
		Log::PrintLog("Failed to Create Listen Socket\n");
		return false;
	}

	SOCKADDR_IN ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons((USHORT)13000);
	inet_pton(AF_INET, "127.0.0.1", &(ServerAddr.sin_addr));

	int reUseAddr = 1;
	::setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reUseAddr, (int)sizeof(reUseAddr));
	
	int result = ::bind(ListenSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr));
	if (result == SOCKET_ERROR)
	{
		Log::PrintLog("Socket Bind Error\n");
		return false;
	}
	
	result = ::listen(ListenSocket, 5);
	if (result == SOCKET_ERROR)
	{
		Log::PrintLog("Listen State Error\n");
		return false;
	}

	Log::PrintLog("Creating Socket Complete\n");

	return true;
}

bool Server::OnAccept(ST_SocketData SocketData)
{
	Session* NewSession = new Session();
	NewSession->OnAccept(SocketData);
	HANDLE Handle = CreateIoCompletionPort((HANDLE)NewSession->SocketData.Socket, this->Iocp, (ULONG_PTR) & (*NewSession), NULL);
	if (!Handle) {
		Log::PrintLog("Fail to Connect IoCompletionPort\n");
		delete NewSession;
		return false;
	}
	SessionManager::Get().AddSession(NewSession);
	NewSession->ReadyToRecieve();
	Log::PrintLog("Connect Client %d\n" , NewSession->SessionID);
	return true;
}

void Server::AcceptWork(Server* ServerPtr)
{
	while (true)
	{
		SOCKET	AcceptSocket = INVALID_SOCKET;
		SOCKADDR_IN RecvAddr;
		static int addrLen = sizeof(SOCKADDR_IN);
		Log::PrintLog("Wait Client\n");
		AcceptSocket = WSAAccept(ListenSocket, (struct sockaddr*)&RecvAddr, &addrLen, NULL, 0);
		if (AcceptSocket == SOCKET_ERROR) {
			Log::PrintLog("Accept Error\n");
			return;
		}
		ST_SocketData NewSocketData;
		NewSocketData.SockAddr = RecvAddr;
		NewSocketData.Socket = AcceptSocket;
		ServerPtr->OnAccept(NewSocketData);
	}
}
