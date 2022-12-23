#pragma once

struct ST_HandleData
{
	E_IOType type = E_IOType::None; 
	char* data = nullptr; 
	UINT32 size = 0;
};

static std::function<void(char*, size_t)> Handler[10];

static void PrintMsg(ST_ChatMessage msg)
{
	//cout << msg.Message << endl;
}

class Client
{
public:
	Client();
	~Client();

	bool CreateClientSocket();
	bool ConnectToServer();

	void SendToServer(char* Msg);
	void SendUserInfo();
	void RecieveWork();
	void Process();

	void SetHandler()
	{
		Handler[E_IOType::S_Chat] = [this](char* data, size_t size) { HandleData<ST_ChatMessage>(PrintMsg, data, size); };
		Handler[E_IOType::S_Chat] = [this](char* data, size_t size) { PrintMsg(GetHandleData<ST_ChatMessage>(data, size)); };

	}
	template<typename PacketType, typename FuncType>
	void HandleData(FuncType func,char* data, size_t size)
	{
		PacketType Data;
		{
			lock_guard<mutex> LockGuard(ReadLock);
			Data = *((PacketType*)(data));
			IODatas[Read].DataPointer += size;
		}
		func(Data);
	}

	template<typename PacketType>
	PacketType GetHandleData(char* data, size_t size)
	{
		PacketType Data;
		{
			lock_guard<mutex> LockGuard(ReadLock);
			Data = *((PacketType*)(data));
			IODatas[Read].DataPointer += size;
		}
		return Data;
	}
 
	
public:

public:
	SOCKET ClientSocket;
	thread RecieveThread;
	ST_IOData IODatas[3];
	list<char*> PopedReadData;
	queue<array<char, BUF_SIZE>> ReadDataQueue;
	queue<shared_ptr<YPacket>> WorkQueue;
	queue<ST_HandleData> HandleDataQueue;
	vector<thread> ProcessThreads;
	condition_variable ProcessCondition;
	Lock QueueLock;
	mutex ProcessLock;
	mutex ReadLock;
private:
	bool Running;
	int count1;
	int count2;
};