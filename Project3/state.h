#pragma once
#include <iostream>  
#include <sstream>
#include <string>
#include <fstream>  
#include <vector>
#include <unordered_map>
#include "winsock2.h" 
#include <time.h>
#include <Windows.h>
#include "MySQLInterface.h"
#include "StateMessage.h"
#include <thread> //thread 头文件,实现了有关线程的类
#include <io.h>
#include <direct.h>
#include "StatesSocket.h"


DWORD WINAPI state_service(LPVOID lpParameter);

