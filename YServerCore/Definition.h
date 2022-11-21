#pragma once

#define BUF_SIZE 4096
#define MAX_CHAT_SIZE 512
#define MAX_ID_LEN 24
#define MAX_PW_LEN 24
#define MAX_RESULT_MSG_LEN 32


#define WriteLock(lock) WriteLockGuard lockguard(lock);
#define ReadLock(lock) ReadLockGuard lockguard(lock);
