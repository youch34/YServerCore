#pragma once

enum E_LockType 
{
	LockGlobal,
	LockSession,
	LockDB
};

class Lock
{
private:
	enum LockFlag : UINT32
	{
		LockTimeOutCount = 10000,
		MaxSpinCount = 5000,
		WriteLockThreadMask = 0xffff0000,
		ReadLockCountMask = 0x0000ffff,
		Empty = 0
	};

	atomic<UINT32> LockFlag = None;
	UINT16 LockCount = 0;
	E_LockType Type;
public:
	Lock() : Type(LockGlobal) {}
	Lock(E_LockType locktype) : Type(locktype) {}
	void TryWriteLock();
	void TryReadLock();
	void TryWriteUnLock();
	void TryReadUnLock();

};

class WriteLockGuard 
{
public:
	WriteLockGuard(Lock& lock) : Lock(lock) { Lock.TryReadLock(); }
	~WriteLockGuard() { Lock.TryReadUnLock(); }
private:
	Lock& Lock;
};
class ReadLockGuard 
{
public:
	ReadLockGuard(Lock& lock) : Lock(lock) { Lock.TryReadLock(); }
	~ReadLockGuard() { Lock.TryReadUnLock(); }
private:
	Lock& Lock;
};
