// Server.cpp : Defines the entry point for the console application.  
//  
#include <iostream> 
#include <thread> //thread ͷ�ļ�,ʵ�����й��̵߳���
#include "winsock2.h"  
#include "MySQLInterface.h"
#include "assignment.h"
#include "ack.h"
#include "tele.h"
using namespace std;

CRITICAL_SECTION g_CS;//ȫ�ֹؼ�����ζ���
int STOP = 0;//�Ƿ�ֹͣsocket��������
int main(int argc, char* argv[])
{
	
	InitializeCriticalSection(&g_CS);//��ʼ���ؼ�����ζ���
    //�����߳�
	HANDLE hThread1;//ACK
	HANDLE hThread2;//�������
	HANDLE hThread3;//ң��
	hThread1 = CreateThread(NULL, 0, ack_service, NULL, 0, NULL);
	hThread2 = CreateThread(NULL, 0, assignment, NULL, 0, NULL);
	hThread3 = CreateThread(NULL, 0, message_rec, NULL, 0, NULL);
	CloseHandle(hThread1);
	CloseHandle(hThread2);
	CloseHandle(hThread3);
	while (1) {
		if (STOP == 1)break;
	}

	Sleep(4000);//���̺߳�����Ĭ4��
	DeleteCriticalSection(&g_CS);//ɾ���ؼ�����ζ���
	system("pause");
	return 0;
}