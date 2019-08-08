#include "state.h"

DWORD state_service(LPVOID lpParameter)
{
	while (1) {
		StatesSocket socket;
		socket.createReceiveServer(5002);
	}
	return 0;
}
