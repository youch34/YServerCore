#pragma once

#define WIN32_LEAN_AND_MEAN
#define BUF_SIZE 1024

#pragma comment(lib,"ws2_32.lib")

#include "CoreLibrary.h"

#pragma comment (lib, "libmysql.lib")
#pragma comment (lib, "YServerCore.lib")

using namespace std;

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <thread>