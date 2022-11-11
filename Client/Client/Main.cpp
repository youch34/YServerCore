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
		cin >> Msg;
		if (strcmp(Msg, "UserInfo") == 0)
			User.SendUserInfo();
		else
			User.SendToServer(Msg);
	}

	return 0;
}