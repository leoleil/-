#define _CRT_SECURE_NO_WARNINGS 1
#include "tele.h"
vector<message_buf> DATA_MESSAGES;//ȫ���ϴ����ݱ��ĳ�
CRITICAL_SECTION data_CS;//�����̳߳عؼ�����ζ���

string getTime()//��ȡʱ���ַ������
{
	time_t timep;
	time(&timep); //��ȡtime_t���͵ĵ�ǰʱ��
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));//�����ں�ʱ����и�ʽ��
	return tmp;
}
string getType(UINT16 num) {
	switch (num)
	{
	case 1:
		return "INT";
	case 2:
		return "INT";
	case 3:
		return "INT";
	case 4:
		return "INT";
	case 5:
		return "BIGINT";
	case 6:
		return "BIGINT";
	case 7:
		return "FLOAT";
	case 8:
		return "DOUBLE";
	case 9:
		return "VARCHAR(1)";
	case 10:
		return "VARCHAR(255)";
	case 11:
		return "VARCHAR(255)";
	case 12:
		return "BLOB";
	case 13:
		return "BLOB";
	case 14:
		return "BLOB";
	case 15:
		return "BOOL";
	case 16:
		return "BIGINT";
	default:
		return "";
	}
}

DWORD message_rec(LPVOID lpParameter)
{
	InitializeCriticalSection(&data_CS);//��ʼ���ؼ�����ζ���
	HANDLE hThread1;//�������ݽ����̣߳���ȡ���ݳ�������
	hThread1 = CreateThread(NULL, 0, message_pasing, NULL, 0, NULL);
	CloseHandle(hThread1);
	while (1) {
		TeleSocket service;//���������������
		service.createReceiveServer(4996, DATA_MESSAGES);
	}
	DeleteCriticalSection(&data_CS);//ɾ���ؼ�����ζ���
	return 0;
}

DWORD message_pasing(LPVOID lpParameter)
{
	const char* SERVER = MYSQL_SERVER.data();//���ӵ����ݿ�ip
	const char* USERNAME = MYSQL_USERNAME.data();
	const char* PASSWORD = MYSQL_PASSWORD.data();
	const char DATABASE[20] = "satellite_teledata";
	const char DATABASE_2[20] = "satellite_message";
	const int PORT = 3306;
	unordered_map<string, int> MAP;//������ǵ�һ����������
	while (1) {
		Sleep(100);
		EnterCriticalSection(&data_CS);//����ؼ������
		if (DATA_MESSAGES.empty()) {
			LeaveCriticalSection(&data_CS);
			continue;
		}
		//���ļ��ϴ�С
		int setLen = DATA_MESSAGES.size();
		LeaveCriticalSection(&data_CS);//�뿪�ؼ������

		for (int i = 0; i < setLen; i++) {
			EnterCriticalSection(&data_CS);//����ؼ������
			char byte_data[70 * 1024];//ÿ�����Ŀռ����70K
			memcpy(byte_data, DATA_MESSAGES[i].val, 70 * 1024);//�����ĳ�������ȡ��
			LeaveCriticalSection(&data_CS);//�뿪�ؼ������
			UINT16 type;//��������
			UINT16 length;//���ĳ���
			memcpy(&type, byte_data, 2);
			memcpy(&length, byte_data + 2, 4);
			//��������
			if (type == 1000) {//���屨��
							   //�������屨��
							   //����ָ��
				int ptr = 6;
							//ʱ���
				long long timestamp;
				memcpy(&timestamp, byte_data + ptr, sizeof(long long));
				ptr = ptr + sizeof(long long);
				//���ܱ�ʶ
				bool flag;
				memcpy(&flag, byte_data + ptr, 1);
				ptr = ptr + 1;
				//������
				UINT32 taskNum;
				memcpy(&taskNum, byte_data + ptr, sizeof(UINT32));
				ptr = ptr + sizeof(UINT32);
				//�豸��
				char name[40];
				memcpy(name, byte_data + ptr, 40);

				//д����־
				MySQLInterface date_db;
				if (date_db.connectMySQL(SERVER, USERNAME, PASSWORD, DATABASE_2, PORT)) {
					string sql_difinition = "insert into ϵͳ��־�� (ʱ��,����,�¼�����,����) values (now(),'ң��ͨ��ģ��',11010,'ͨ�����Ļ��յ�";
					sql_difinition = sql_difinition + name + "ң�ⱨ���屨��');";
					date_db.writeDataToDB(sql_difinition);
					date_db.closeMySQL();
				}

				cout << "| ����ң��         | ";
				cout << getTime();
				cout << "| ���ն��屨��";
				cout.setf(ios::left);
				cout.width(37);
				cout << name;
				cout << "|" << endl;
				MAP[name] = 0;//����豸
				ptr = ptr + 40;
				//���豸��
				char parentName[40];
				memcpy(parentName, byte_data + ptr, 40);
				ptr = ptr + 40;
				//���Ǳ��
				char satillitId[20];
				memcpy(satillitId, byte_data + ptr, 20);
				ptr = ptr + 20;
				//ʶ���ֶ�����
				vector<field> fields;
				while (ptr < length) {
					field sub_field;
					//�ֶ���
					memcpy(sub_field.name, byte_data + ptr, 40);
					ptr = ptr + 40;
					//�ֶ����ͱ��
					memcpy(&sub_field.type, byte_data + ptr, 2);
					ptr = ptr + 2;
					//��Сֵ
					memcpy(&sub_field.min, byte_data + ptr, 8);
					ptr = ptr + 8;
					//���ֵ
					memcpy(&sub_field.max, byte_data + ptr, 8);
					ptr = ptr + 8;
					//��λ
					memcpy(sub_field.unit, byte_data + ptr, 8);
					ptr = ptr + 8;
					//��ʾ��ʶ
					memcpy(&sub_field.display, byte_data + ptr, 1);
					ptr = ptr + 1;
					fields.push_back(sub_field);
				}
				//������

				MySQLInterface db;
				//�������ݿ�
				if (db.connectMySQL(SERVER, USERNAME, PASSWORD, DATABASE, PORT)) {
					//��ѯ�Ƿ��иö����ֶ�
					string f_sql = "select * from �ֶζ���� where �豸�� =";
					f_sql = f_sql + "'" + name + "'and ���Ǳ��='"+ satillitId+"';";
					vector<vector <string>>res;
					if (!db.getDatafromDB(f_sql, res)) {
						cout << db.errorNum << endl;
						cout << db.errorInfo << endl;
					}
					if (res.size() != 0) {
						cout << name << "�豸�ڶ�������Ѿ�����" << endl;
						//���ñ���ɾ��
						EnterCriticalSection(&data_CS);//����ؼ������
													   //���ĳ�-1
						DATA_MESSAGES.erase(DATA_MESSAGES.begin() + i);
						i--;
						LeaveCriticalSection(&data_CS);//�뿪�ؼ������

					}
					else {
						//�������򴴽�

						//�����ֶζ���
						for (int i = 0; i < fields.size(); i++) {
							//���������sql���
							string sql = "insert into �ֶζ���� (�ֶ���,��������,���ֵ,��Сֵ,��λ,��ʾ��־,�豸��,���Ǳ��) values(";
							sql = sql + "'" + fields[i].name + "',";
							sql = sql + to_string(fields[i].type) + ",";
							//���û�������Сֵ
							if (fields[i].max == NEW_NULL) {
								sql = sql + "NULL,";
							}
							else {
								sql = sql + to_string(fields[i].max) + ",";
							}
							if (fields[i].min == NEW_NULL) {
								sql = sql + "NULL,";
							}
							else {
								sql = sql + to_string(fields[i].min) + ",";
							}

							sql = sql + "'" + fields[i].unit + "',";
							sql = sql + to_string(fields[i].display) + ",";
							sql = sql + "'" + name + "','"+satillitId+"');";
							if (!db.writeDataToDB(sql)) {
								cout << db.errorNum << endl;
								cout << db.errorInfo << endl;
							}
						}
						//���������ݵı�
						string d_sql = "create table ";
						d_sql = d_sql+ satillitId+"_" + name + " (���� INT AUTO_INCREMENT,ʱ�� BIGINT,";
						for (int i = 0; i < fields.size(); i++) {
							d_sql = d_sql + fields[i].name + " " + getType(fields[i].type);
							if (i != fields.size() - 1)d_sql = d_sql + ",";
						}
						d_sql = d_sql + ",primary key(����));";
						//cout << d_sql << endl;
						if (!db.writeDataToDB(d_sql)) {
							//cout << db->errorNum << endl;
							//cout << db->errorInfo << endl;
						}
						//������ϵ��
						string r_sql = "insert into �豸��ϵ��(�豸��,���豸��,���Ǳ��) values('";
						r_sql = r_sql + name + "','" + parentName + "','"+ satillitId +"');";
						if (!db.writeDataToDB(r_sql)) {
							//cout << db->errorNum << endl;
							//cout << db->errorInfo << endl;
						}
						
						//���ñ���ɾ��
						EnterCriticalSection(&data_CS);//����ؼ������
													//���ĳ�-1
						DATA_MESSAGES.erase(DATA_MESSAGES.begin() + i);
						i--;
						LeaveCriticalSection(&data_CS);//�뿪�ؼ������

					}
					//�ر�����
					db.closeMySQL();
				}
				else {
					//cout << db->errorNum << endl;
					//cout << db->errorInfo << endl;
				}

			}
			else if (type == 2000) {
				//�������ݱ���
				//����ָ��
				int ptr = 6;
							//ʱ���
				long long timestamp;
				memcpy(&timestamp, byte_data + ptr, sizeof(long long));
				ptr = ptr + sizeof(long long);
				//���ܱ�ʶ
				bool flag;
				memcpy(&flag, byte_data + ptr, sizeof(bool));
				ptr = ptr + sizeof(bool);
				UINT32 taskNum;
				memcpy(&taskNum, byte_data + ptr, sizeof(UINT32));
				ptr = ptr + sizeof(UINT32);
				//�豸��
				char name[40];
				memcpy(name, byte_data + ptr, 40);
				ptr = ptr + 40;

				//д��־
				if (MAP[name] == 0) {
					MAP[name] = 1;
					MySQLInterface date_db;
					if (date_db.connectMySQL(SERVER, USERNAME, PASSWORD, DATABASE_2, PORT)) {
						string sql_data = "insert into ϵͳ��־�� (ʱ��,����,�¼�����,����) values (now(),'ң��ͨ��ģ��',11011,'ͨ�����Ļ��յ���һ��";
						sql_data = sql_data + name + "ң�ⱨ�����ݱ���');";
						cout << "| ͨ��ģ�� | ";
						cout << getTime();
						cout << "| �������ݱ���";
						cout.setf(ios::left);
						cout.width(37);
						cout << name;
						cout << "|" << endl;
						date_db.writeDataToDB(sql_data);
						date_db.closeMySQL();
					}
					
				}
				//���Ǳ��
				char satillitId[20];
				memcpy(satillitId, byte_data + ptr, 20);
				ptr = ptr + 20;
				//����ʱ��
				long long getTime;
				memcpy(&getTime, byte_data + ptr, sizeof(long long));
				ptr = ptr + sizeof(long long);
				//������ݿ��д��ֶ�
				//ʶ���������
				MySQLInterface db;
				//�������ݿ�
				if (db.connectMySQL(SERVER, USERNAME, PASSWORD, DATABASE, PORT)) {

					//������
					//��ѯ�ֶ���������
					vector<vector <string>>res;
					string ff_sql = "select ��������,�ֶ��� from �ֶζ���� where �豸��=";
					ff_sql = ff_sql + "'" + name + "'and ���Ǳ��='"+ satillitId +"' order by id;";
					if (db.getDatafromDB(ff_sql, res)) {
						if (res.size() == 0) {
							cout << name << "�豸������" << endl;
						}
						//���ݿ�������
						else {
							string sql = "insert into ";
							sql = sql + satillitId + "_" + name + "(ʱ��,";
							string d_sql = " values(";
							d_sql = d_sql + to_string(timestamp) + ",";
							for (int i = 0; i < res.size(); i++) {
								//���ͺ���
								UINT16 code = 0;
								StringNumUtils util;
								code = util.stringToNum<UINT16>(res[i][0]);
								sql = sql + res[i][1];
								if (i < res.size() - 1)sql = sql + ",";
								if (i == res.size() - 1)sql = sql + ")";
								if (code == 1) {//16λ����
									INT16 data = 0;
									memcpy(&data, byte_data + ptr, sizeof(INT16));
									ptr = ptr + sizeof(INT16);
									d_sql = d_sql + to_string(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 2) {//16λ�޷�������
									UINT16 data = 0;
									memcpy(&data, byte_data + ptr, sizeof(UINT16));
									ptr = ptr + sizeof(UINT16);
									d_sql = d_sql + to_string(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 3) {//32λ����
									INT32 data = 0;
									memcpy(&data, byte_data + ptr, sizeof(INT32));
									ptr = ptr + sizeof(INT32);
									d_sql = d_sql + to_string(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 4) {//32λ�޷�������
									UINT32 data = 0;
									memcpy(&data, byte_data + ptr, sizeof(UINT32));
									ptr = ptr + sizeof(UINT32);
									d_sql = d_sql + to_string(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 5) {//64λ����
									INT64 data = 0;
									memcpy(&data, byte_data + ptr, sizeof(INT64));
									ptr = ptr + sizeof(INT64);
									d_sql = d_sql + to_string(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 6) {//64λ�޷�������
									UINT64 data = 0;
									memcpy(&data, byte_data + ptr, sizeof(UINT64));
									ptr = ptr + sizeof(UINT64);
									d_sql = d_sql + to_string(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 7) {//�����ȸ�����ֵ
									float data = 0;
									memcpy(&data, byte_data + ptr, sizeof(float));
									ptr = ptr + sizeof(float);
									d_sql = d_sql + to_string(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 8) {//˫���ȸ�����ֵ
									double data = 0;
									memcpy(&data, byte_data + ptr, sizeof(double));
									ptr = ptr + sizeof(double);
									d_sql = d_sql + to_string(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 9) {//�ַ�����
									char data;
									memcpy(&data, byte_data + ptr, sizeof(char));
									ptr = ptr + sizeof(char);
									d_sql = d_sql + "'" + data + "'";
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 10) {//���ַ�������
									char data[15];
									memcpy(&data, byte_data + ptr, sizeof(char) * 15);
									ptr = ptr + sizeof(char) * 15;
									d_sql = d_sql + "'" + data + "'";
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 11) {//���ַ�������
									char data[255];
									memcpy(&data, byte_data + ptr, sizeof(char) * 255);
									ptr = ptr + sizeof(char) * 255;
									d_sql = d_sql + "'" + data + "'";
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								//��bug
								else if (code == 12) {//���ֽ�����
									char data[255];
									memcpy(&data, byte_data + ptr, sizeof(char) * 255);
									ptr = ptr + sizeof(char) * 255;
									d_sql = d_sql + "'" + data + "'";
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 13) {//���ֽ�����
									char data[32 * K];
									memcpy(&data, byte_data + ptr, sizeof(char) * 32 * K);
									ptr = ptr + sizeof(char) * 32 * K;
									d_sql = d_sql + "'" + data + "'";
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 14) {//���ֽ�����
									char data[60 * K];
									memcpy(&data, byte_data + ptr, sizeof(char) * 60 * K);
									ptr = ptr + sizeof(char) * 60 * K;
									d_sql = d_sql + "'" + data + "'";
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 15) {//������
									bool data;
									memcpy(&data, byte_data + ptr, sizeof(bool));
									ptr = ptr + sizeof(bool);
									d_sql = d_sql + to_string(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else if (code == 16) {//ʱ���
									long long data;
									memcpy(&data, byte_data + ptr, sizeof(long long));
									ptr = ptr + sizeof(long long);
									d_sql = d_sql + to_string(data);
									if (i != res.size() - 1)d_sql = d_sql + ",";
								}
								else {
									cout << "δ�����������ʹ���" << endl;
								}
							}
							d_sql = d_sql + ");";
							sql = sql + d_sql;
							//cout << sql << endl;
							if (!db.writeDataToDB(sql)) {
								//cout << db->errorNum << endl;
								//cout << db->errorInfo << endl;
							}
							//Sleep(10);
							EnterCriticalSection(&data_CS);//����ؼ������
														//���ĳ�-1
							DATA_MESSAGES.erase(DATA_MESSAGES.begin() + i);
							i--;
							LeaveCriticalSection(&data_CS);//�뿪�ؼ������
						}

					}
					else {
						//cout << db->errorNum << endl;
						//cout << db->errorInfo << endl;
					}
					//�ر�����
					db.closeMySQL();
				}
				else {
					//cout << db->errorNum << endl;
					//cout << db->errorInfo << endl;
				}
			}
			else {
				
			}

			//���±��ĳصĴ�С
			//Sleep(1);
			EnterCriticalSection(&data_CS);//����ؼ������
			setLen = (int)DATA_MESSAGES.size();
			//system("cls");
			//cout << "Ŀǰ���ĳش�С��" << MESSAGE_VECTOR.size() << endl;
			LeaveCriticalSection(&data_CS);//�뿪�ؼ������
		}

	}
	return 0;
}
