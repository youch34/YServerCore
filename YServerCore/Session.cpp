#include "stdafx.h"
#include "Session.h"

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
	IODatas[Read].CurBytes += TransferSize;
	if (IODatas[Read].TotalBytes == 0)
	{
		if (TransferSize > sizeof(ST_IOHeader))
		{
			ST_IOHeader* Header = (ST_IOHeader*)IODatas[Read].Data;
			IODatas[Read].TotalBytes = Header->Size;
			WSABUF Buffer = { 0, };
			Buffer.buf = IODatas[Read].Data + IODatas[Read].CurBytes;
			Buffer.len = IODatas[Read].TotalBytes - (ULONG)IODatas[Read].CurBytes;
		}
	}
	if (IODatas[Read].CurBytes < IODatas[Read].TotalBytes)
	{
		WSABUF Buffer = { 0, };
		Buffer.buf = IODatas[Read].Data + IODatas[Read].CurBytes;
		Buffer.len = BUF_SIZE - (ULONG)IODatas[Read].CurBytes;
		Recv(Buffer);
		return;
	}

	PacketProcessor.Processing(this , E_IOUsage::Read);

	this->ReadyToRecieve();
}

void Session::ReadyToRecieve()
{
	ZeroMemory(&IODatas[Read], sizeof(ST_IOData));
	IODatas[Read].Usage = E_IOUsage::Read;
	WSABUF Buffer = {0,};
	Buffer.buf = IODatas[Read].Data;
	Buffer.len = BUF_SIZE;
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
	IODatas[Write].CurBytes += TransferSize;
	if (IODatas[Write].TotalBytes == 0)
	{
		if (TransferSize > sizeof(ST_IOHeader))
		{
			ST_IOHeader* Header = (ST_IOHeader*)IODatas[Write].Data;
			IODatas[Write].TotalBytes = Header->Size;
			WSABUF Buffer = { 0, };
			Buffer.buf = IODatas[Write].Data + IODatas[Write].CurBytes;
			Buffer.len = IODatas[Write].TotalBytes - (ULONG)IODatas[Write].CurBytes;
		}
	}
	if ((IODatas[Write].CurBytes += TransferSize) < IODatas[Write].TotalBytes)
	{
		WSABUF Buffer = {0,};
		Buffer.buf = IODatas[Write].Data + IODatas[Write].CurBytes;
		Buffer.len = BUF_SIZE - (ULONG)IODatas[Write].CurBytes;
		Send(Buffer);
	}
	Log::PrintLog("Packet Send");
	ZeroMemory(&IODatas[Write], sizeof(ST_IOData));
}

void Session::SendPacket(char* Packet)
{
	int size = sizeof(Packet);
	ZeroMemory(&IODatas[Write], sizeof(ST_IOData));
	IODatas[Write].Usage = E_IOUsage::Write;
	ST_IOHeader* Header = (ST_IOHeader*)Packet;
	WSABUF Buffer = {0, };
	memcpy(IODatas[Write].Data, Packet, Header->Size);
	Buffer.buf = IODatas[Write].Data;
	Buffer.len = Header->Size;
	Send(Buffer);
}

void Session::Send(WSABUF Buffer)
{
	DWORD SendBytes = 0;
	DWORD flags = 0;
	int result = WSASend(SocketData.Socket, &Buffer, 1, &SendBytes, flags, &IODatas[Write].Overlapped, NULL);
}

