#pragma once
#include <winsock2.h> 
#include "MessageParser.h"
#pragma comment(lib, "ws2_32.lib")   
using namespace std;

extern  vector <message_byte> MESSAGE_VECTOR;//���ĳ�
extern CRITICAL_SECTION g_CS;//ȫ�ֹؼ�����ζ���
extern int STOP;//ֹͣ��ʶ

//ͨ�Žӿ�;
DWORD WINAPI SocketServicr(LPVOID lpParameter);
//���ķָ�����ķָ���뱨�ĳ���
int messageCut(BYTE * message, int length);
//���뱨�ĳ�
void addMessageSet(message_byte message);