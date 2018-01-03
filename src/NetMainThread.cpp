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
		Command * c = newCommand(&p);
		pthread_t workerThread;
		pthread_create(&workerThread, NULL, &commandExeWrapper, static_cast<void *>(c));
	}
}

