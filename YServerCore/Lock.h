#pragma once


class Lock
{
private:
	enum LockFlag : UINT32
	{
		LockTimeOutCount = 10000,
		MaxSpinCount = 5000,
		None = 0
	};
	Lock() {}

	atomic<UINT32> LockFlag = None;
	UINT16 LockCount = 0;
private:
	~Lock() { UnLock(); }
	void UnLock();
public:
	void TryLock();
public:
	static Lock SessionLock;
	static Lock DBLock;
};
