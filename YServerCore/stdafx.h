#pragma once

#define WIN32_LEAN_AND_MEAN

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "libmysql.lib")

using namespace std;


#include <stdio.h>
#include <map>
#include <future>
#include <shared_mutex>
#include <unordered_map>
#include <thread>
#include <Windows.h>
#include <functional>
#include <iostream>
#include <queue>
#include <list>
#include <vector>
#include <string>
#include <stdarg.h>
#include <atomic>
#include <sysinfoapi.h>
#include <cassert>
#include <mutex>
#include <array>

#include "Log.h"
#include "Definition.h"

#include "CoreTLS.h"
#include "PacketDataStructure.h"
#include "Database.h"
#include "Lock.h"

