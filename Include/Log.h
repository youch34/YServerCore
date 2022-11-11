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
	void printLog(string str) { printf("%s\n", str.c_str()); };
	void printLog(float Num) { printf("%.2f\n", Num); }
	void printLog(const char* str, float Num) { printf("%s : ", str); printLog(Num); }
	//void PrintLog(const char* str, ...) 
	//{
	//	va_list args;
	//	int count = 0;
	//	va_start(args, str);
	//	while (str[count] != '\0')
	//	{
	//		PrintLog(va_arg(args, char*));
	//		count++;
	//	}
	//	va_end(args);
	//
	//};
	void printWSALastError()
	{
		if (WSAGetLastError() == ERROR_IO_PENDING)
			return;
		char Error[1024];
		_itoa_s(WSAGetLastError(), Error, 10);
		printLog(Error);
	}
};