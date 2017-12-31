#include <stdio.h>
#include <stdlib.h>

#include "FileTransfer.h"

void * commandExeWrapper(void * command)
{
	Command * cmd = static_cast<Command *>(command);
	cmd->execute();
	delete cmd;
	return 0;
}

Command * newCommand(Packet * p)
{
	Command * outCommand;
	if(p->opcodeRequestType == Type::FileTransfer)
		outCommand = new FileTransfer(p);
	else if(...)
		;
	else
		;
	return outCommand;
}

int main(void)
{
	// Get commands from UDP
	while(1)
	{
		Packet p = udpSocket.getPacket();
		Command * c = newCommand(&p);
		pthread_t workerThread;
		pthread_create(&workerThread, NULL, &commandExeWrapper, static_cast<void *>(c));
	}

	return 0;
}
