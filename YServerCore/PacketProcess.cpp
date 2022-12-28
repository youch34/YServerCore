#include "stdafx.h"
#include "PacketProcess.h"
#include "SessionManager.h"




PacketProcess::PacketProcess()
{
	SetHandler();
}

PacketProcess::~PacketProcess()
{
}

void RecvCreateAccountRequest(Session* session, ST_UserInfo userinfo)
{
	ST_UserInfo Packet;
	if (Database::Get().IsValidID(userinfo.Id))
	{
		Packet.Header.Success = false;
		Packet.Header.SetHeader("Already Exist ID");
		session->SendPacket((char*)&Packet);
		return;
	}
	bool result = Database::Get().CreateAccount(userinfo);
	Packet.Header.Success = result;
	result ? Packet.Header.SetHeader("Success") : Packet.Header.SetHeader("Unknown Error");
	session->SendPacket((char*)&Packet);
}

void RecvLoginRequest(Session* session, ST_UserInfo userinfo)
{
	bool result = Database::Get().IsValidAccount(userinfo);
	userinfo.Header.SetHeader(E_IOType::S_Login, result, result ? "" : "Unkwon Account");
	if (result)
	{
		session->SetUserInfo(userinfo);
		session->SetSessionState(E_SessionState::Login);
		SessionManager::Get().AllMessage((char*)&userinfo);
	}
	else
	{
		session->SendPacket((char*)&userinfo);
	}
}

void RecvChatMessage(Session* session, ST_ChatMessage msg)
{
	msg.Header.SetHeader(E_IOType::S_Chat);
	Log::PrintLog("%s : %s", msg.Id, msg.Message);
	//SessionManager::Get().AllMessage((char*)&msg);
}

void RecvMoveRequest(Session* session, ST_MoveInfo moveinfo)
{
	moveinfo.Header.SetHeader(E_IOType::S_Move);
	moveinfo.SetID(session->UserInfo.Id);
	SessionManager::Get().AllMessage((char*)&moveinfo);
}