#include "stdafx.h"
#include "Thread.h"
#include "Server.h"


Thread::Thread()
{
	MaxThreadCount = thread::hardware_concurrency()*2 + 1;
	//MaxThreadCount = 1;
}

Thread::~Thread()
{
	for (int i = 0; i < MaxThreadCount; i++)
	{
		ThreadPool[i].join();
	}
}

bool Thread::Initialize()
{
	if (RunningServer == nullptr)
	{
		Log::PrintLog("Need Set Server");
		return false;
	}
	for (int i = 0; i < MaxThreadCount; i++)
	{
		ThreadPool[i] = thread(&Thread::NetworkProcess, this);
	}
	Log::PrintLog("%d count Thread Created" , MaxThreadCount);
	bRunning = true;
	return bRunning;
}

void Thread::InitLocalThread()
{
	static atomic<UINT32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void Thread::AddWork(function<void(void)> func)
{
	Lock.lock();
	JopQ.push(func);
	Condition.notify_one();
	Lock.unlock();
}

void Thread::AddWork2(void(*func)(void))
{
	Lock.lock();
	JopQ2.push(func);
	Condition.notify_one();
	Lock.unlock();
}

void Thread::AddData(Data* data)
{
	Lock.lock();
	DataQ.push(data);
	Condition.notify_one();
	Lock.unlock();
}

void Thread::Work()
{
	while (bRunning)
	{
		unique_lock<mutex> ULock(Lock);
		Condition.wait(ULock, [this] { return !DataQ.empty(); });
		if (!DataQ.empty())
		{
			Data* data = DataQ.front();
			cout << data->num << endl;
			DataQ.pop();
		}
	}

}

void Thread::NetworkProcess()
{
	InitLocalThread();
	while (bRunning)
	{
		ST_IOData* IoData = nullptr;
		Session* Session = nullptr;
		DWORD transferSize;
		bool result = GetQueuedCompletionStatus(RunningServer->Iocp, &transferSize, (PULONG_PTR)&Session, (LPOVERLAPPED*)&IoData, INFINITE);
		if (transferSize == 0 || !result)
			continue;

		switch (IoData->Usage)
		{
			case E_IOUsage::Read :
			{
				Session->OnRecv(transferSize);
				continue;
			}
			case E_IOUsage::Write :
			{
				Session->OnSend(transferSize);
				continue;
			}
			break;
		}
	}
}
