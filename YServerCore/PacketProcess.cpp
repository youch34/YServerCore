#include "stdafx.h"
#include "PacketProcess.h"
#include "Session.h"
#include "SessionManager.h"
PacketProcess::PacketProcess()
{
}

PacketProcess::~PacketProcess()
{
}

void PacketProcess::Processing(Session* session, enum E_IOUsage Usage)
{

	auto Header = GetHeader(session->IODatas[Read].Data);
	switch (Header->IOType)
	{
		case E_IOType::C_CreateAccount:
		{
			auto UserInfo = CastedIO<ST_UserInfo>(session->IODatas[Read].Data);
			RecvCreateAccountRequest(session, *UserInfo);
			break;
		}
		case E_IOType::C_Login:
		{
			auto UserInfo = CastedIO<ST_UserInfo>(session->IODatas[Read].Data);
			RecvLoginRequest(session ,*UserInfo);
			break;
		}
		case E_IOType::C_Connect:
		{
			break;
		}
		case E_IOType::C_Chat:
		{
			RecvChatMessage(session, nullptr);
			break;
		}
		case E_IOType::C_Move:
		{
			auto MoveInfo = CastedIO<ST_MoveInfo>(session->IODatas[Read].Data);
			RecvMoveRequest(session, *MoveInfo);
			break;
		}
		
	}
}

void PacketProcess::RecvCreateAccountRequest(class Session* session, ST_UserInfo userinfo)
{
	auto Packet = CreateHeader(E_IOType::S_CreateAccount);
	if (Database::Get().IsValidID(userinfo))
	{
		Packet.SetHeader(false);
		Packet.SetHeader("ID used Already");
		session->SendPacket((char*)&Packet);
		return;
	}
	bool result = Database::Get().CreateAccount(userinfo);
	Packet.SetHeader(result);
	result ? Packet.SetHeader("Sucess") : Packet.SetHeader("Unknown Error");
	session->SendPacket((char*)&Packet);

}

void PacketProcess::RecvLoginRequest(Session* session, ST_UserInfo userinfo)
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

void PacketProcess::RecvChatMessage(Session* session, const char* Msg)
{
	auto Message = CastedIO<ST_ChatMessage>(session->IODatas[Read].Data);
	Message->Header.SetHeader(E_IOType::S_Chat);
	Log::PrintLog(Message->Message);
	SessionManager::Get().AllMessage((char*)Message);
}

void PacketProcess::RecvMoveRequest(Session* session, ST_MoveInfo moveinfo)
{
	moveinfo.Header.SetHeader(E_IOType::S_Move);
	moveinfo.SetID(session->UserInfo.Id);
	SessionManager::Get().AllMessage((char*)&moveinfo);
}