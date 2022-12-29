#include "stdafx.h"
#include "Session.h"
#include "PacketProcess.h"
#include "SessionManager.h"



Session::Session()
{

}

Session::~Session()
{
	closesocket(SocketData.Socket);
}

void Session::OnAccept(ST_SocketData NewSocketData)
{
	SocketData = NewSocketData;
	int reUseAddr = 1;
	::setsockopt(SocketData.Socket, SOL_SOCKET, SO_REUSEADDR, (char*)&reUseAddr, (int)sizeof(reUseAddr));
	SetIODataUsage();
}

void Session::OnRecv(size_t TransferSize)
{
	if(TransferSize == 0)
		SessionManager::Get().CloseSession(this);

	IODatas[Read].CurBytes += TransferSize;

	while (IODatas[Read].DataPointer < IODatas[Read].CurBytes)
	{
		if ((IODatas[Read].CurBytes - IODatas[Read].DataPointer) < sizeof(ST_IOHeader))
			break;

		ST_IOHeader* Header = (ST_IOHeader*)(IODatas[Read].Data + IODatas[Read].DataPointer);

		if ((IODatas[Read].CurBytes - IODatas[Read].DataPointer) < Header->Size)
			break;

		if (PacketProcess::Get().Handler[Header->IOType])
		{
			PacketProcess::Get().Handler[Header->IOType](this, IODatas[Read].Data + IODatas[Read].DataPointer, Header->Size);
		}
		else
			break;
	}
	if (IODatas[Read].CurBytes == BUF_SIZE)
	{
		memcpy(IODatas[Read].Data, IODatas[Read].Data + IODatas[Read].DataPointer, BUF_SIZE - IODatas[Read].DataPointer);
		IODatas[Read].CurBytes = BUF_SIZE - IODatas[Read].DataPointer;
		IODatas[Read].DataPointer = 0;
	}
	else
	{
		IODatas[Read].CurBytes = 0;
		IODatas[Read].DataPointer = 0;
	}
	this->ReadyToRecieve();
}

void Session::ReadyToRecieve()
{
	IODatas[Read].Usage = E_IOUsage::Read;
	WSABUF Buffer = {0,};
	Buffer.buf = IODatas[Read].Data + IODatas[Read].CurBytes;
	Buffer.len = BUF_SIZE - IODatas[Read].CurBytes;
	this->Recv(Buffer);
}

void Session::Recv(WSABUF Buffer)
{
	DWORD RecvBytes = 0; 
	DWORD flags = 0;
	int result = WSARecv(SocketData.Socket, &Buffer, 1, &RecvBytes, &flags, &IODatas[Read].Overlapped, NULL);
}

void Session::OnSend(size_t TransferSize)
{
	if (TransferSize == 0)
		SessionManager::Get().CloseSession(this);
}

void Session::SendPacket(char* Packet)
{
	if (Packet == nullptr)
		return;

	{
		std::unique_lock<std::shared_mutex> lock(S_Mutex);
		ST_IOHeader* Header = (ST_IOHeader*)Packet;
		if (IODatas[Write].CurBytes + Header->Size > BUF_SIZE)
		{
			Sleep(10);
			this->SendPacket(Packet);
			return;
		}
		memcpy(IODatas[Write].Data + IODatas[Write].CurBytes, Packet, Header->Size);
		IODatas[Write].CurBytes += Header->Size;
	}
	std::unique_lock<std::shared_mutex> lock(S_Mutex);
	Send();
}

void Session::Send()
{	
	DWORD SendBytes = 0;
	DWORD flags = 0;
	WSABUF Buffer{ 0, };
	Buffer.buf = IODatas[Write].Data;
	Buffer.len = IODatas[Write].CurBytes;
	int result = WSASend(SocketData.Socket, &Buffer, 1, &SendBytes, flags, &IODatas[Write].Overlapped, NULL);
	IODatas[Write].CurBytes = 0;
	if (WSAGetLastError() == SOCKET_ERROR)
	{
		Log::PrintLog("Session Send Error\n");
		SessionManager::Get().CloseSession(this);
	}
}

