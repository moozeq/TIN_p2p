#include "NetMainThread.h"

Command * NetMainThread::newCommand(UdpPacket * p)
{
	Command * outCommand;
	if(p->task == "fileTransfer")
	{
		outCommand = new FileTransfer();
	}
	else if(true)
		;
	else
		;
	return outCommand;
}

void NetMainThread::execute(void)
{
	UdpSocket udpSocket;
	// Get commands from UDP
	while(1)
	{
		UdpPacket p = udpSocket.getPacket();
		Command * command = newCommand(&p);
		if(command != nullptr)
		{
			if(command->reqSeparateThread())
			{
				pthread_t thread;
				pthread_create(&thread, NULL, Command::commandExeWrapper, static_cast<void *>(command));
			}
			else
			{
				command->execute();
				delete command;
			}
		}
	}
}

