#include "cancelAssignment.h"

DWORD cancelAssigement(LPVOID lpParameter)
{
	const char SERVER[10] = "127.0.0.1";
	const char USERNAME[10] = "root";
	const char PASSWORD[10] = "";
	const char DATABASE[20] = "satellite";
	const int PORT = 3306;
	while (1) {
		//5�������ݿ����������
		Sleep(5000);
		MySQLInterface* mysql = new MySQLInterface();
		if (mysql->connectMySQL(SERVER, USERNAME, PASSWORD, DATABASE, PORT)) {
			string selectSql = "select ������ from �������� where �ַ���־ = 3";
			vector<vector<string>> dataSet;//��ȡ��������
			mysql->getDatafromDB(selectSql, dataSet);
			if (dataSet.size() == 0) {
				continue;//��Ҫ��������Ĭ5��������ѯ
			}
			for (int i = 0, len = dataSet.size(); i < len; i++) {
				char* messageDate;
				StringNumUtils * util = new StringNumUtils();

				UINT16 messageId = 4010;//���ı�ʶ
				long long dateTime = Message::getSystemTime();//��ȡ��ǰʱ���
				bool encrypt = false;//�Ƿ����
				UINT32 taskNum = util->stringToNum<UINT32>(dataSet[i][0]);//������
				UINT16 taskType = util->stringToNum<UINT16>(dataSet[i][1]);//��������
				long long taskStartTime = util->stringToNum<long long>(dataSet[i][2]);//�ƻ���ʼʱ��
				long long taskEndTime = util->stringToNum<long long>(dataSet[i][3]);//�ƻ�����ʱ��
				char* satelliteId = new char[20];//���Ǳ��
				strcpy_s(satelliteId, dataSet[i][4].size() + 1, dataSet[i][4].data());
				char* groundStationId = new char[20];//����վ���
													 //dataSet[i][5].copy(groundStationId, dataSet[i][5].size(), 0);
				strcpy_s(groundStationId, dataSet[i][5].size() + 1, dataSet[i][5].data());
				AllocationMessage * message = new AllocationMessage(messageId, dateTime, encrypt, taskNum, taskType, taskStartTime, taskEndTime, satelliteId, groundStationId);

				//���ҵ���վip��ַ���ͱ���
				string groundStationSql = "select IP��ַ from ����վ��Ϣ�� where ����վ��� =" + dataSet[i][5];
				vector<vector<string>> ipSet;
				mysql->getDatafromDB(groundStationSql, ipSet);
				if (ipSet.size() == 0)continue;//û���ҵ�ip��ַ
				Socket* socketer = new Socket();
				const char* ip = ipSet[0][0].data();//��ȡ����ַ
				if (socketer->createSendServer(ip, 9000, 0))continue;
				const int bufSize = message->getterMessageLength();
				int returnSize = 0;
				char* sendBuf = new char[bufSize];//���뷢��buf
				message->createMessage(sendBuf, returnSize, bufSize);
				if (socketer->sendMessage(sendBuf, returnSize) == -1)continue;
				cout << "| �������         | " << dataSet[i][0] << "���������ɹ�" << endl;

				//�޸����ݿ�ַ���־
				string ackSql = "update �������� set �ַ���־ = 2 where task_number = " + dataSet[i][0];
				mysql->writeDataToDB(ackSql);

			}

		}
		else {
			cout << "| �������         | �������ݿ�ʧ��" << endl;
			cout << "| ������������Ϣ | " << mysql->errorNum << endl;
		}
		cout << "| �������         | ����������" << endl;

	}
}
