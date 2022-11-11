#pragma once

#define WIN32_LEAN_AND_MEAN
#define BUF_SIZE 1024
#define MAX_CHAT_SIZE 512
#define MAX_ID_LEN 24
#define MAX_PW_LEN 24
#define MAX_RESULT_MSG_LEN 32
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "libmysql.lib")

using namespace std;

#include <stdio.h>
#include <map>
#include <thread>
#include <Windows.h>
#include <functional>
#include <iostream>
#include <queue>
#include <list>
#include <vector>
#include <string>
#include <stdarg.h>

#include "Log.h"

#include "PacketDataStructure.h"
#include "Database.h"