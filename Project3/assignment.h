#pragma once
#include <iostream>  
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "winsock2.h" 
#include <time.h>
#include <Windows.h>
#include "MySQLInterface.h"
#include "Message.h"
#include "AllocationMessage.h"
#include "StringNumUtils.h"
#include "Socket.h"

using namespace std;

extern string MYSQL_SERVER ;//���ӵ����ݿ�ip
extern string MYSQL_USERNAME;
extern string MYSQL_PASSWORD;

/*�������ģ��*/
//��������߳����
DWORD WINAPI assignment(LPVOID lpParameter);

