#include "stdafx.h"
#include "Client.h"

void PrintMessage(const char* msg, int loop) 
{
	for (int i = 0; i < loop; i++) 
	{
		Log::PrintLog(msg);
	}
}
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
		else
		{
			ST_ChatMessage Message;
			Message.Header.SetHeader(E_IOType::C_Chat);
			Message.Header.Size = sizeof(ST_ChatMessage);
			Message.SetMessage("Client", Msg);
			User.SendToServer((char*)&Message);
		}
	}



	return 0;
}