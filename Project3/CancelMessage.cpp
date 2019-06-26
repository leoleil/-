#include "CancelMessage.h"



CancelMessage::CancelMessage(UINT16 messageId, long long dateTime, bool encrypt, UINT32 taskNum):
Message(messageId, dateTime, encrypt), taskNum(taskNum)
{
	this->messageLength = this->messageLength + sizeof(UINT32);
}


CancelMessage::~CancelMessage()
{
}

UINT32 CancelMessage::getterTaskNum()
{
	return this->taskNum;
}

void CancelMessage::createMessage(char * buf, int & message_size, int buf_size)
{
	if (buf_size >= messageLength) {
		char* bufPtr = buf;//bufָ��
		memcpy(bufPtr, &(messageId), sizeof(UINT16));//�����־
		bufPtr = bufPtr + sizeof(UINT16);//�ƶ�ָ��
		memcpy(bufPtr, &messageLength, sizeof(UINT32));//���ĳ���
		bufPtr = bufPtr + sizeof(UINT32);//�ƶ�ָ��
		memcpy(bufPtr, &dateTime, sizeof(long long));//ʱ���
		bufPtr = bufPtr + sizeof(long long);//�ƶ�ָ��
		memcpy(bufPtr, &encrypt, sizeof(bool));//���ܱ�ʶ
		bufPtr = bufPtr + sizeof(bool);//�ƶ�ָ��
		memcpy(bufPtr, &taskNum, sizeof(UINT32));//������
		message_size = messageLength;
	}
}

void CancelMessage::messageParse(char * buf)
{
	char* bufPtr = buf;//bufָ��
	memcpy(&(messageId), bufPtr, sizeof(UINT16));//�����־
	bufPtr = bufPtr + sizeof(UINT16);//�ƶ�ָ��
	memcpy(&messageLength, bufPtr, sizeof(UINT32));//���ĳ���
	bufPtr = bufPtr + sizeof(UINT32);//�ƶ�ָ��
	memcpy(&dateTime, bufPtr, sizeof(long long));//ʱ���
	bufPtr = bufPtr + sizeof(long long);//�ƶ�ָ��
	memcpy(&encrypt, bufPtr, sizeof(bool));//���ܱ�ʶ
	bufPtr = bufPtr + sizeof(bool);//�ƶ�ָ��
	memcpy(&taskNum, bufPtr, sizeof(UINT32));//������
}
