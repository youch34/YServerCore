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

	//PacketProcessor.Processing(this , E_IOUsage::Read);

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
	if ((IODatas[Write].CurBytes += TransferSize) < IODatas[Write].TotalBytes)
	{
		WSABUF Buffer = {0,};
		Buffer.buf = IODatas[Write].Data + IODatas[Write].CurBytes;
		Buffer.len = IODatas[Write].TotalBytes - (ULONG)IODatas[Write].CurBytes;
		Send();
	}
	//if (!SendQueue.empty())
	//{
	//	Send();
	//	return;
	//}
	ZeroMemory(&IODatas[Write], sizeof(ST_IOData));
	IODatas[Write].Usage = E_IOUsage::Write;
	IODatas[Write].Complete.store(false);
}

void Session::SendPacket(char* Packet)
{
	if (Packet == nullptr)
		return;

	{
		//WriteLock(QueueLock);
		lock_guard<mutex> lg(Qmutex);
		ST_IOHeader* Header = (ST_IOHeader*)Packet;
		shared_ptr<YPacket> SendPacket(static_cast<YPacket*>(malloc(Header->Size)), free);
		memcpy(SendPacket.get(), Packet, Header->Size);
		SendQueue.push(SendPacket);
	}

	bool Expected = false;
	if (IODatas[Write].Complete.compare_exchange_strong(Expected, true))
	{
		//WriteLock(QueueLock);
		lock_guard<mutex> lg(Qmutex);
		Send();
	}

}

void Session::Send()
{	
	char* DataPointer = IODatas[Write].Data;
	int DataSize = 0;
	while (!SendQueue.empty())
	{
		ST_IOHeader* Header = (ST_IOHeader*)SendQueue.front().get();
		if (DataSize + Header->Size > BUF_SIZE)
			break;
		memcpy(DataPointer, SendQueue.front().get(), Header->Size);
		DataPointer += Header->Size;
		DataSize += Header->Size;
		Log::PrintLog("S_SendCount : %d", Header->Size);
		SendQueue.pop();
	}
	DWORD SendBytes = 0;
	DWORD flags = 0;
	WSABUF Buffer{ 0, };
	Buffer.buf = IODatas[Write].Data;
	Buffer.len = DataSize;
	IODatas[Write].TotalBytes = DataSize;
	int result = WSASend(SocketData.Socket, &Buffer, 1, &SendBytes, flags, &IODatas[Write].Overlapped, NULL);
}

