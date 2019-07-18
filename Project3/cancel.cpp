#include "cancel.h"

DWORD cancel(LPVOID lpParameter)
{
	const char SERVER[10] = "127.0.0.1";//���ӵ����ݿ�ip
	const char USERNAME[10] = "root";
	const char PASSWORD[10] = "";
	const char DATABASE[20] = "satellite_message";
	const int PORT = 3306;
	while (1) {
		//5�������ݿ����������
		Sleep(5000);
		cout << "| �������ģ��     | ������ݿ�����..." << endl;
		MySQLInterface mysql;
		if (mysql.connectMySQL(SERVER, USERNAME, PASSWORD, DATABASE, PORT)) {
			string selectSql = "select ������,��������,unix_timestamp(�ƻ���ʼʱ��),unix_timestamp(�ƻ���ֹʱ��),���Ǳ��,����վ��� from �������� where �ַ���־ = 3";
			vector<vector<string>> dataSet;//��ȡ��������
			mysql.getDatafromDB(selectSql, dataSet);
			if (dataSet.size() == 0) {
				continue;//������Ĭ5��������ѯ
			}

			for (int i = 0, len = dataSet.size(); i < len; i++) {
				//�������ݽ��տռ�
				char* messageDate;
				int messageDataSize = 0;
				if (dataSet[i][1]._Equal("101")) {
					//�����ң�ط��䱨����Ҫ�鱨������
					string dadaSql = "select �������� from ң�����ݱ� where ������ =" + dataSet[i][0];
					vector<vector<string>> dataBlob;
					mysql.getDatafromDB(dadaSql, dataBlob);
					if (dataBlob.size() == 0)continue;
					messageDataSize = dataBlob[0][0].size() + 1;
					messageDate = new char[messageDataSize];//����ռ�
					strcpy_s(messageDate, messageDataSize, dataBlob[0][0].c_str());
				}

				//ת������
				StringNumUtils util;

				UINT16 messageId = 4020;//���ı�ʶ
				long long dateTime = Message::getSystemTime();//��ȡ��ǰʱ���
				bool encrypt = false;//�Ƿ����
				UINT32 taskNum = util.stringToNum<UINT32>(dataSet[i][0]);//������
				UINT16 taskType = util.stringToNum<UINT16>(dataSet[i][1]);//��������
				long long taskStartTime = util.stringToNum<long long>(dataSet[i][2]);//�ƻ���ʼʱ��
				//����Ѿ�����ִ��ʱ��
				if (taskStartTime > Message::getSystemTime()) {
					string ackSql = "update �������� set �ַ���־ = 5 where ������ = " + dataSet[i][0];
					continue;
				}
				long long taskEndTime = util.stringToNum<long long>(dataSet[i][3]);//�ƻ�����ʱ��
				char* satelliteId = new char[20];//���Ǳ��
				strcpy_s(satelliteId, dataSet[i][4].size() + 1, dataSet[i][4].c_str());
				char* groundStationId = new char[20];//����վ���
				strcpy_s(groundStationId, dataSet[i][5].size() + 1, dataSet[i][5].c_str());
				AllocationMessage message(messageId, dateTime, encrypt, taskNum, taskType, taskStartTime, taskEndTime, satelliteId, groundStationId);
				if (messageDataSize > 0) {
					message.setterMessage(messageDate, messageDataSize);
					delete messageDate;
				}
				//�ͷſռ�
				delete groundStationId;
				delete satelliteId;


				//���ҵ���վip��ַ���ͱ���
				string groundStationSql = "select IP��ַ from ����վ��Ϣ�� where ����վ��� =" + dataSet[i][5];
				vector<vector<string>> ipSet;
				mysql.getDatafromDB(groundStationSql, ipSet);
				if (ipSet.size() == 0)continue;//û���ҵ�ip��ַ
				Socket socketer;
				const char* ip = ipSet[0][0].c_str();//��ȡ����ַ
				if (!socketer.createSendServer(ip, 4998, 0))continue;
				const int bufSize = 66560;//65K����
				int returnSize = 0;
				char sendBuf[bufSize];//���뷢��buf
				ZeroMemory(sendBuf, bufSize);//��0
				message.createMessage(sendBuf, returnSize, bufSize);
				if (socketer.sendMessage(sendBuf, bufSize) == -1)continue;

				cout << "| �������ģ��     | " << dataSet[i][0] << "���������ɹ�" << endl;

				socketer.offSendServer();
				//�޸����ݿ�ַ���־
				string ackSql = "update �������� set �ַ���־ = 4 where ������ = " + dataSet[i][0];
				mysql.writeDataToDB(ackSql);

			}
			mysql.closeMySQL();
		}
		else {
			cout << "| �������         | �������ݿ�ʧ��" << endl;
			cout << "| ������������Ϣ | " << mysql.errorNum << endl;
		}
		cout << "| �������         | ����������" << endl;

	}
}
