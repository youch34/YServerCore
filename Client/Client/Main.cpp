#include "stdafx.h"
#include "Client.h"
int main() 
{
	Client User;
	User.CreateClientSocket();
	User.ConnectToServer();

	while (true)
	{
		char Msg[MAX_CHAT_SIZE] = "";
		fgets(Msg, MAX_CHAT_SIZE, stdin);
		if (strcmp(Msg, "UserInfo") == 0)
			User.SendUserInfo();
		else if (strcmp(Msg, "m"))
		{
			ST_ChatMessage Message;
			Message.Header.SetHeader(E_IOType::S_Chat);
			Message.Header.Size = sizeof(ST_ChatMessage);
			Message.SetMessage("Server", "TESTTEST");
			array<char, BUF_SIZE> ReadData{0,};
		
		}
		else
			User.SendToServer(Msg);
	}

	return 0;
}