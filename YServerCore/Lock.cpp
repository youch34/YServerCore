#include "stdafx.h"
#include "Lock.h"


void Lock::TryWriteLock()
{
	if ((LockFlag & WriteLockThreadMask) >> 16 == LThreadId)
	{
		LockCount++;
		return;
	}

	const INT64 BeginTick = GetTickCount64();
	const UINT32 Desired = ((LThreadId << 16) & WriteLockThreadMask);

	while (true)
	{
		for (UINT32 SpinCount = 0; SpinCount < MaxSpinCount; SpinCount++)
		{
			UINT32 Expected = Empty;
			if (LockFlag.compare_exchange_strong(Expected, Desired))
			{
				LockCount++;
				return;
			}
		}

		if (GetTickCount64() - BeginTick > LockTimeOutCount)
		{
			Log::PrintLog("Doubt Dead Lock!! Try Lock Failed");
				assert("MULTIPLE_UNLOCK");
		}
		this_thread::yield();
	}
}

void Lock::TryWriteUnLock()
{
	if ((LockFlag & ReadLockCountMask) != 0)
		assert("Invalid unlock order");

	const UINT16 LastLockCount = --LockCount;
	if (LastLockCount == 0)
		LockFlag.store(Empty);
}

void Lock::TryReadLock() 
{
	if ((LockFlag & WriteLockThreadMask) >> 16 == LThreadId)
	{
		LockFlag.fetch_add(1);
		return;
	}
	const INT64 BeginTick = GetTickCount64();

	while (true)
	{
		for (UINT32 SpinCount = 0; SpinCount < MaxSpinCount; SpinCount++)
		{
			UINT32 Expected = (LockFlag.load() & ReadLockCountMask);
			if (LockFlag.compare_exchange_strong(Expected, Expected + 1))
			{
				LockCount++;
				return;
			}
		}

		if (GetTickCount64() - BeginTick > LockTimeOutCount)
		{
			Log::PrintLog("Doubt Dead Lock!! Try Lock Failed");
				assert("MULTIPLE_UNLOCK");
		}
		this_thread::yield();
	}
}

void Lock::TryReadUnLock() 
{
	if((LockFlag.fetch_sub(1) & ReadLockCountMask) == 0)
		assert("MULTIPLE_UNLOCK");
}