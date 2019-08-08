#pragma once
#include "Socket.h"
class StatesSocket :
	public Socket
{
public:
	StatesSocket();
	~StatesSocket();
public:
	int createReceiveServer(const int port);
};

