#include "stdafx.h"
#include "Lock.h"

Lock Lock::DBLock;
Lock Lock::SessionLock;

void Lock::TryLock()
{
	if ((LThreadId & LockFlag) == LThreadId)
	{
		LockCount++;
		return;
	}

	const INT64 BeginTick = GetTickCount64();
	const UINT32 Desired = LockFlag & LThreadId;

	while (true)
	{
		for (UINT32 SpinCount = 0; SpinCount < MaxSpinCount; SpinCount++)
		{
			UINT32 Expected = None;
			if (LockFlag.compare_exchange_strong(Expected, Desired))
			{
				LockCount++;
				return;
			}
		}

		if (GetTickCount64() - BeginTick > LockTimeOutCount)
		{
			Log::PrintLog("Doubt Dead Lock!! Try Lock Failed");
			return;
		}
		this_thread::yield();
	}
}

void Lock::UnLock()
{
	const UINT16 LastLockCount = --LockCount;
	if (LastLockCount == 0)
		LockFlag.store(None);
}
