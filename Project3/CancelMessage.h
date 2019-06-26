#pragma once
#include "Message.h"
class CancelMessage :
	public Message
{
public:
	CancelMessage(UINT16 messageId, long long dateTime, bool encrypt, UINT32 taskNum);
	~CancelMessage();
private:
	UINT32 taskNum;
public:
	UINT32 getterTaskNum();
	//Ϊ���ݴ��䴴��һ��message�Ľӿ�
	void createMessage(char* buf, int & message_size, int buf_size);
	//ͨ�����ݰ����������ݽӿ�
	void messageParse(char* buf);
};

