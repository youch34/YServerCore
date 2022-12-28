#pragma once

struct ST_HandleData
{
	E_IOType type = E_IOType::None; 
	UINT32 size = 0;
	char data[BUF_SIZE];
};

static std::function<void(char*, size_t)> Handler[10];

static void PrintMsg(ST_ChatMessage msg)
{
	static int count = 1;
	++count;
	for (int i = 1; i < count; i++)
	{
		Log::PrintLog("%d\n", i);
	}
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
	void Process(ST_HandleData data);

	void SetHandler()
	{
		Handler[E_IOType::S_Chat] = [this](char* data, size_t size) { HandleData<ST_ChatMessage>(PrintMsg, data, size); };
	}
	template<typename PacketType, typename FuncType>
	void HandleData(FuncType func,char* data, size_t size)
	{
		PacketType Data;
		{
			Data = *((PacketType*)(data));
			IODatas[Read].DataPointer += size;
		}
		std::make_unique<std::future<void>*>(new auto(std::async(func, Data))).reset();
	}
 
	
public:
	queue<ST_HandleData> HandleDataQueue;

public:
	SOCKET ClientSocket = NULL;
	thread RecieveThread;
	ST_IOData IODatas[3];
	mutex ReadLock;
private:
	bool Running = false;
};