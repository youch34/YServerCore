#pragma once
#include "stdafx.h"

enum E_IOType
{
	None,
	C_CreateAccount,
	S_CreateAccount,
	C_Login,
	S_Login,
	C_Connect,
	S_Connect,
	C_Chat,
	S_Chat,
	C_Move,
	S_Move,
};

struct YPacket
{

};

struct ST_IOHeader
{
	E_IOType IOType = E_IOType::None;
	UINT32 Size = sizeof(ST_IOHeader);
	bool Success = false;
	char IOResultInfo[MAX_RESULT_MSG_LEN] = {0,};
	void SetHeader(E_IOType iotype)
	{
		IOType = iotype;
	}
	void SetHeader(bool val)
	{
		Success = val;
	}
	void SetHeader(string resultmsg)
	{
		strcpy_s(IOResultInfo, resultmsg.c_str());
	}
	void SetHeader(E_IOType iotype,bool val, string resultmsg)
	{
		SetHeader(iotype);
		SetHeader(resultmsg);
		SetHeader(val);
	}
};

struct ST_UserInfo : YPacket
{
	ST_IOHeader Header{ E_IOType::S_Connect, sizeof(ST_UserInfo) };
	char Id[MAX_ID_LEN];
	char Pw[MAX_PW_LEN];
	void SetInfo(string id, string pw)
	{
		strcpy_s(Id,id.c_str());
		strcpy_s(Pw, pw.c_str());
	}
};

struct ST_ChatMessage : YPacket
{
	ST_IOHeader Header{ E_IOType::None, sizeof(ST_ChatMessage) };
	char Id[MAX_ID_LEN];
	char Message[MAX_CHAT_SIZE];
	void SetMessage(string id, string Msg)
	{
		strcpy_s(Id, id.c_str());
		strcpy_s(Message, Msg.c_str());
	}
};

struct ST_MoveInfo : YPacket
{
	ST_IOHeader Header{ E_IOType::None, sizeof(ST_MoveInfo) };
	char Id[MAX_ID_LEN];
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	void SetVector(float X, float Y, float Z)
	{
		x = X; y = Y; z = Z;
	}
	void SetID(string id)
	{
		strcpy_s(Id, id.c_str());
	}
};