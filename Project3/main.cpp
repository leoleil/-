// Server.cpp : Defines the entry point for the console application.  
//  
#include <iostream> 
#include <thread> //thread ͷ�ļ�,ʵ�����й��̵߳���
#include "winsock2.h"  
#include "MessageParser.h"
#include "Database.h"
#include "socket.h"
#include "MySQLInterface.h"
using namespace std;

vector <message_byte> MESSAGE_VECTOR;//ȫ�ֱ��ĳ�
CRITICAL_SECTION g_CS;//ȫ�ֹؼ�����ζ���
int STOP = 0;//�Ƿ�ֹͣsocket��������

template <class Type> string numToString(Type& num)
{
	stringstream ss;
	ss << num;
	string s1 = ss.str();
	return s1;
}
template <class Type> Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}

int main(int argc, char* argv[])
{
	
	InitializeCriticalSection(&g_CS);//��ʼ���ؼ�����ζ���
    //�����߳�
	HANDLE hThread1;
	HANDLE hThread2;
	hThread1 = CreateThread(NULL, 0, messagePasing, NULL, 0, NULL);
	hThread2 = CreateThread(NULL, 0, SocketServicr, NULL, 0, NULL);
	CloseHandle(hThread1);
	CloseHandle(hThread2);
	
	while (1) {

	}

	Sleep(4000);//���̺߳�����Ĭ4��
	DeleteCriticalSection(&g_CS);//ɾ���ؼ�����ζ���
	
	return 0;
}