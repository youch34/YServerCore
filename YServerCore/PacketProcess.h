#pragma once

class PacketProcess 
{
public:
	PacketProcess();
	~PacketProcess();

	void Processing(class Session* session, enum E_IOUsage Usage);
public:
	void RecvCreateAccountRequest(class Session* session, ST_UserInfo userinfo);
	void RecvConnectRequest(ST_UserInfo userinfo);
	void RecvLoginRequest(Session* session,ST_UserInfo userinfo);
	void RecvChatMessage(class Session* session, const char* Msg);
	void RecvMoveRequest(class Session* session, ST_MoveInfo moveinfo);

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
};