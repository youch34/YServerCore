#include "CoreLibrary.h"
#pragma comment (lib, "libmysql.lib")
#pragma comment (lib, "YServerCore.lib")

int main()
{
	Server MyServer;
	if (!MyServer.Run())
	{
		return false;
	}
	ST_UserInfo info;
	info.SetInfo("Player0", "0000");
	Database::Get().IsValidAccount(info);
	while (true)
	{
		char Msg[MAX_CHAT_SIZE];
		fgets(Msg, MAX_CHAT_SIZE, stdin);
		//scanf_s("%s", Msg, MAX_CHAT_SIZE);
		static ST_ChatMessage Message;
		Message.Header.SetHeader(E_IOType::S_Chat);
		Message.Header.Size = sizeof(ST_ChatMessage);
		Message.SetMessage("Server", Msg);
		SessionManager::Get().AllMessage((char*)&Message);
	}
	return 0;
}