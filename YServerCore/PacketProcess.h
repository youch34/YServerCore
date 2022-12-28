#pragma once
#include "Session.h"


static void PrintMsg(ST_ChatMessage msg)
{
	//cout << msg.Message << endl;
}

void RecvCreateAccountRequest(Session* session, ST_UserInfo userinfo);
void RecvLoginRequest(Session* session,ST_UserInfo userinfo);
void RecvChatMessage(Session* session, ST_ChatMessage msg);
void RecvMoveRequest(Session* session, ST_MoveInfo moveinfo);

class PacketProcess 
{
private:
	PacketProcess();
	~PacketProcess();

public:
	static PacketProcess& Get()
	{
		static PacketProcess Instance;
		return Instance;
	}
public:

	template<class T>
	T* CastedIO(char* Packet) { T* result = (T*)Packet; return result; }

	bool GetSuccess(char* Packet) { auto Header = CastedIO<ST_IOHeader>(Packet); return Header->Success; }
	ST_IOHeader* GetHeader(char* Packet) { auto Header = CastedIO<ST_IOHeader>(Packet); return Header; }
	ST_IOHeader CreateHeader(E_IOType type, bool success = true) 
	{
		ST_IOHeader Packet;
		Packet.SetHeader(type);
		Packet.SetHeader(success);
		return Packet;
	};
	ST_IOHeader CreateHeader(E_IOType type, string result,bool success = true)
	{
		ST_IOHeader Packet;
		Packet.SetHeader(type);
		Packet.SetHeader(success);
		Packet.SetHeader(result);
		return Packet;
	};

public:
	std::function<void(class Session*, char*, size_t)> Handler[20];
public:
	void SetHandler()
	{
		
		Handler[E_IOType::C_Chat] = [this](class Session* session, char* data, size_t size) { HandleData<ST_ChatMessage>(RecvChatMessage, session,data, size);};
		Handler[E_IOType::C_CreateAccount] = [this](class Session* session, char* data, size_t size) { HandleData<ST_UserInfo>(RecvCreateAccountRequest, session, data, size); };
		Handler[E_IOType::C_Login] = [this](class Session* session, char* data, size_t size) { HandleData<ST_UserInfo>(RecvLoginRequest, session, data, size); };
		Handler[E_IOType::C_Move] = [this](class Session* session, char* data, size_t size) { HandleData<ST_MoveInfo>(RecvMoveRequest, session, data, size); };
	}


	template<typename PacketType, typename FuncType>
	void HandleData(FuncType func,class Session* session,char* data, size_t size)
	{
		PacketType Data;
		{
			Data = *((PacketType*)(data));
			session->IODatas[Read].DataPointer += size;
		}
		std::make_unique<std::future<void>*>(new auto(std::async(func, session,Data))).reset();
	}
};