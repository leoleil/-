/*
����������
�������������ݿ��������������������ݷ��͸���صĵ���վ���ն�
*/

#include "assignment.h"

DWORD WINAPI assignment(LPVOID lpParameter) {
	const char* SERVER = MYSQL_SERVER.data();//���ӵ����ݿ�ip
	const char* USERNAME = MYSQL_USERNAME.data();
	const char* PASSWORD = MYSQL_PASSWORD.data();
	const char DATABASE[20] = "uav_message";
	const int PORT = 3306;
	while (1) {
		//5�������ݿ����������
		Sleep(5000);
		//cout << "| �������ģ��     | ������ݿ�����..." << endl;
		MySQLInterface mysql;
		if (mysql.connectMySQL(SERVER, USERNAME, PASSWORD, DATABASE, PORT)) {
			string selectSql = "select ������,��������,unix_timestamp(��עʱ��),��������.���˻����,ǰ�û����, ����״̬, ����ָ�� from ��������,���˻���Ϣ�� where ����״̬ = 2 || ����״̬ = 4 and ��������.���˻���� = ���˻���Ϣ��.���˻����;";
			vector<vector<string>> dataSet;//��ȡ��������
			mysql.getDatafromDB(selectSql, dataSet);
			if (dataSet.size() == 0) {
				continue;//������Ĭ5��������ѯ
			}

			for (int i = 0,len = dataSet.size() ; i < len ; i++) {
				//�������ݽ��տռ�
				char* messageDate;
				int messageDataSize = 0;
				//ת������
				StringNumUtils util;
				//if (dataSet[i][1]._Equal("2")) {
				//	//����Ǽƻ�����
				//	long long dateTime = Message::getSystemTime();//��ȡ��ǰʱ���
				//	long long taskStartTime = util.stringToNum<long long>(dataSet[i][2]);//�ƻ���עʱ��
				//	if (taskStartTime * 1000 > dateTime) {
				//		//�����û���ƻ���ʼʱ�������
				//		continue;
				//	}
				//}
				messageDataSize = dataSet[i][6].size() + 1;
				messageDate = new char[messageDataSize];//����ռ�
				strcpy_s(messageDate, messageDataSize, dataSet[i][6].c_str());
				
				UINT16 messageId = 4010;//���ı�ʶ
				if (dataSet[i][5]._Equal("4"))messageId = 4020;//����ǳ���
				long long dateTime = Message::getSystemTime();//��ȡ��ǰʱ���
				bool encrypt = false;//�Ƿ����
				UINT32 taskNum = util.stringToNum<UINT32>(dataSet[i][0]);//������
				UINT16 taskType = util.stringToNum<UINT16>(dataSet[i][1]);//��������
				long long taskStartTime = util.stringToNum<long long>(dataSet[i][2]);//��עʱ��
				char* satelliteId = new char[20];//���˻����
				strcpy_s(satelliteId, dataSet[i][3].size()+1, dataSet[i][3].c_str());
				char* groundStationId = new char[20];//ǰ�û����
				strcpy_s(groundStationId, dataSet[i][4].size()+1, dataSet[i][4].c_str());
				AllocationMessage message (messageId, dateTime, encrypt, taskNum, taskType, taskStartTime, satelliteId);//����
				if (messageDataSize > 0) { 
					message.setterMessage(messageDate, messageDataSize);
					delete messageDate;
				}
				//�ͷſռ�
				delete groundStationId;
				delete satelliteId;
				

				//���ҵ���վip��ַ���ͱ���
				string groundStationSql = "select IP��ַ from ǰ�û���Ϣ�� where ǰ�û���� ='" + dataSet[i][4] + "';";
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
				socketer.offSendServer();
				//�޸����ݿ�ַ���־
				string ackSql = "update �������� set ����״̬ = 3, ����ʱ�� = now() where ������ = " + dataSet[i][0];
				string logSql  = "insert into ϵͳ��־�� (ʱ��,����,�¼�����,����) values (now(),'�������ģ��',15000,'"+dataSet[i][0] + "���������ɹ� ');";
				if (dataSet[i][5]._Equal("4")) { 
					ackSql = "update �������� set ����״̬ = 5 where ������ = " + dataSet[i][0]; 
					cout << "| �������ģ��     | " << dataSet[i][0] << "������������" << endl;
					logSql = "insert into ϵͳ��־�� (ʱ��,����,�¼�����,����) values (now(),'�������ģ��',15000,'" + dataSet[i][0] + "������������ ');";
				}
				else {
					cout << "| �������ģ��     | " << dataSet[i][0] << "���������ɹ�" << endl;
				}
				mysql.writeDataToDB(ackSql);
				mysql.writeDataToDB(logSql);
				
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
