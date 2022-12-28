#include "CoreLibrary.h"
#pragma comment (lib, "libmysql.lib")
#pragma comment (lib, "YServerCore.lib")
#include <fstream>

void Send() 
{
	for (int i = 0; i < 500; i++)
	{
		ST_ChatMessage Message;
		Message.Header.SetHeader(E_IOType::S_Chat);
		Message.Header.Size = sizeof(ST_ChatMessage);
		Message.SetMessage("Server", "Msg");
		SessionManager::Get().AllMessage((char*)&Message);
	}
}

int main()
{
	Server MyServer;

	if (!MyServer.Run())
	{
		return false;
	}
	
	Sleep(3000);
	thread t1 = thread(Send);
	thread t2 = thread(Send);
	thread t3 = thread(Send);
	thread t4 = thread(Send);
	
	
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	


	while (true)
	{
		char Msg[MAX_CHAT_SIZE];
		fgets(Msg, MAX_CHAT_SIZE, stdin);
		ST_ChatMessage Message;
		Message.Header.SetHeader(E_IOType::S_Chat);
		Message.Header.Size = sizeof(ST_ChatMessage);
		Message.SetMessage("Server", Msg);
		SessionManager::Get().AllMessage((char*)&Message);
	}

	return 0;
}