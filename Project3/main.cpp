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
string MYSQL_SERVER = "";//���ӵ����ݿ�ip
string MYSQL_USERNAME = "";
string MYSQL_PASSWORD = "";
int main(int argc, char* argv[])
{
	ifstream is("config.txt", ios::in);
	if (!is.is_open()) {
		cout << "| ���ݿ�����       | ";
		cout << " �޷��������ļ�������config.txt�Ƿ����ã�������ϵͳ" << endl;
		//�������ɹ��ͷ���Դ
		system("pause");
		return 0;
	}
	getline(is, MYSQL_SERVER);
	getline(is, MYSQL_USERNAME);
	getline(is, MYSQL_PASSWORD);
	is.close();
	InitializeCriticalSection(&g_CS);//��ʼ���ؼ�����ζ���
    //�����߳�
	HANDLE hThread1;//ACK
	hThread1 = CreateThread(NULL, 0, ack_service, NULL, 0, NULL);
	CloseHandle(hThread1);
	HANDLE hThread2;//�������
	hThread2 = CreateThread(NULL, 0, assignment, NULL, 0, NULL);
	CloseHandle(hThread2);
	HANDLE hThread3;//ң��
	hThread3 = CreateThread(NULL, 0, message_rec, NULL, 0, NULL);
	CloseHandle(hThread3);
	while (1) {
		if (STOP == 1)break;
	}

	Sleep(4000);//���̺߳�����Ĭ4��
	DeleteCriticalSection(&g_CS);//ɾ���ؼ�����ζ���
	system("pause");
	return 0;
}