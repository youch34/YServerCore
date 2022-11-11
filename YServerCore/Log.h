#pragma once
#include "stdafx.h"
#include <WinSock2.h>
class Log 
{
public:
	Log() {};
	~Log() {};

	//Log(const Log& rhs) = delete;

	static Log Get()
	{
		static Log instance;
		return instance;
	}

public:
	static void PrintLog(const char* str, ...) 
	{
		va_list args;
		va_start(args, str);
		vprintf(str,args);
		va_end(args);
		printf("\n");
	};
	void printWSALastError()
	{
		if (WSAGetLastError() == ERROR_IO_PENDING)
			return;
		char Error[1024];
		_itoa_s(WSAGetLastError(), Error, 10);
		PrintLog(Error);
	}
};