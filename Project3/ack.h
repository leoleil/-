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
#include "ACKMessage.h"
#include <thread> //thread ͷ�ļ�,ʵ�����й��̵߳���
#include <io.h>
#include <direct.h>
#include "AckSocket.h"

DWORD WINAPI ack_service(LPVOID lpParameter);