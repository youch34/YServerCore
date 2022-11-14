#pragma once
#include <mutex>
class Server;

class Data 
{
public:
	Data(int _num) { num = _num; }
public:
	int num = 0;
};

class Thread
{
public:
	Thread();
	~Thread();

public:
	void SetServer(Server* ServerPtr) { RunningServer = ServerPtr; }
	bool Initialize();
	void InitLocalThread();
	void CloseThread() { bRunning = false; };

	void AddWork(function<void(void)> func);
	void AddWork2(void(*func)(void));
	void AddData(Data* data);

	void Work();

	void NetworkProcess();
public:
	map<int, class thread> ThreadPool;
	queue<function<void(void)>> JopQ;
	queue<void(*)(void)> JopQ2;
	queue<Data*> DataQ;
	int MaxThreadCount = 0;

	Server* RunningServer = nullptr;

private:
	bool bRunning = false;
	mutex Lock;
	condition_variable Condition;
};