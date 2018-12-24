#include"socket.h"
#include "MySQLInterface.h"
const int BUF_SIZE = 10*M;			//����buffer��С
WSADATA         wsd;            //WSADATA����  
SOCKET          sServer;        //�������׽���  
SOCKET          sClient;        //�ͻ����׽���  
SOCKADDR_IN     addrServ;;      //��������ַ  
char            buf[BUF_SIZE];  //�������ݻ�����  
char            sendBuf[BUF_SIZE];//���ظ��ͻ��˵�����  
int             retVal;         //����ֵ  

DWORD WINAPI SocketServicr(LPVOID lpParameter)
{

	//��ʼ���׽��ֶ�̬��  
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		cout << "WSAStartup failed!" << endl;
		return 1;
	}

	//�����׽���  
	sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sServer)
	{
		cout << "socket failed!" << endl;
		WSACleanup();//�ͷ��׽�����Դ;  
		return  -1;
	}

	//�������׽��ֵ�ַ   
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(4999);
	addrServ.sin_addr.s_addr = INADDR_ANY;
	//���׽���  
	retVal = bind(sServer, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == retVal)
	{
		cout << "bind failed!" << endl;
		closesocket(sServer);   //�ر��׽���  
		WSACleanup();           //�ͷ��׽�����Դ;  
		return -1;
	}

	//��ʼ����   
	cout << "listening ..." << endl;
	retVal = listen(sServer, 1);
	
	if (SOCKET_ERROR == retVal)
	{
		cout << "listen failed!" << endl;
		closesocket(sServer);   //�ر��׽���  
		WSACleanup();           //�ͷ��׽�����Դ;  
		return -1;
	}

	//���ܿͻ�������  
	sockaddr_in addrClient;
	int addrClientlen = sizeof(addrClient);
	sClient = accept(sServer, (sockaddr FAR*)&addrClient, &addrClientlen);
	if (INVALID_SOCKET == sClient)
	{
		cout << "accept failed!" << endl;
		closesocket(sServer);   //�ر��׽���  
		WSACleanup();           //�ͷ��׽�����Դ;  
		return -1;
	}
	cout << "accept success!" << endl;
	cout << "data from cline" << endl;
	MySQLInterface* db_test = new MySQLInterface;
	if (db_test->connectMySQL("127.0.0.1", "root", "", "mangeinfodb", 3306)) {
		db_test->writeDataToDB("insert into ϵͳ��־�� (����,�¼�����,����) values ('ͨ��ģ��',1010,'�����뷢�Ͷ˽���TCP����');");
		db_test->closeMySQL();
	}
	else {
		//����ʧ��
		cout << db_test->errorNum << ":" << db_test->errorInfo << endl;
		return 0;
	}
	while (true)
	{
		EnterCriticalSection(&g_CS);//����ؼ������
		if (STOP == 1)break;
		LeaveCriticalSection(&g_CS);//�뿪�ؼ������
		//���տͻ�������
		//���buffer
		ZeroMemory(buf, BUF_SIZE);
		//��ȡ����
		retVal = recv(sClient, buf, BUF_SIZE, 0);
		
		if (SOCKET_ERROR == retVal)
		{
			cout << "recv failed!" << endl;
			closesocket(sServer);   //�ر��׽���
			closesocket(sClient);   //�ر��׽���       
			WSACleanup();           //�ͷ��׽�����Դ;  
			return -1;
		}
		cout << "��ȡ" << retVal << "�ֽ�" << endl;
		
		Sleep(10);
		
		//��buf����ֽڿ���
		BYTE byte_buf[BUF_SIZE];
		memcpy(byte_buf, buf, BUF_SIZE);
		
		//���ñ��Ľ����ӿ�
		messageCut(byte_buf, BUF_SIZE);
		
	}
	if (db_test->connectMySQL("127.0.0.1", "root", "", "mangeinfodb", 3306)) {
		db_test->writeDataToDB("insert into ϵͳ��־�� (����,�¼�����,����) values ('�����뷢�Ͷ˶Ͽ�TCP����');");
		db_test->closeMySQL();
	}
	else {
		//����ʧ��
		cout << db_test->errorNum << ":" << db_test->errorInfo << endl;
		return 0;
	}
	system("cls");
	cout << "����ر�,���ݴ������" << endl;
	//�˳�  
	closesocket(sServer);   //�ر��׽���  
	closesocket(sClient);   //�ر��׽���  
	WSACleanup();           //�ͷ��׽�����Դ;  
	return 0;
}
//��ȡ�����Ӵ������뱨�ĳ���
int messageCut(BYTE * message, int size) {
	BYTE * ptr = message;
	UINT16 length=0;
	for (int i = 0; i < size - 1; i = i + length) {

		if (ptr[i] == 0X00 && ptr[i + 1] == 0X00)return 0;
		//��ȡ���ĳ���
		memcpy(&length, ptr + i + 2, 2);
		//cout << "len:" << length << endl;
		//��ȡһ��buffer
		message_byte buffer;
		//�ڴ渴��
		memcpy(buffer.val, ptr + i, length);
		//���뱨�ĳ�
		//Sleep(5);
		EnterCriticalSection(&g_CS);//����ؼ������
		MESSAGE_VECTOR.push_back(buffer);
		//cout << "ĿǰMESSAGE_VECTOR��" << MESSAGE_VECTOR.size() << endl;
		LeaveCriticalSection(&g_CS);//�뿪�ؼ������
	}
	return 0;
}
