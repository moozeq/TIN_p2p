#include "NetMainThread.h"
#include "Request.h"
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <sys/uio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BROAD "127.255.255.255"
#define PORT 8888   //The port on which to send data

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

void die(std::string s)
{
    perror(s.c_str());
    exit(1);
}

int init(void)
{
    struct sockaddr_in si_other;
    int s;
	socklen_t slen = sizeof(si_other);

	//socket
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        die("socket");

    //broadcast
	int broadcast=1;
	if (setsockopt(s,SOL_SOCKET,SO_BROADCAST, &broadcast,sizeof(broadcast))==-1) {
		die(strerror(errno));
	}

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);

    if (inet_aton(BROAD , &si_other.sin_addr) == 0)
        die("inet_aton() failed\n");

	Request * req = new Request(100);
	if (sendto(s, req, sizeof(*req), 0, (struct sockaddr*) &si_other, slen) == -1)
		die("sendto() joining network");
	std::cout<<"Sent req, opcode = " << req->opcode <<std::endl;
	delete req;

    return 0;
}

void NetMainThread::execute(void)
{
	init();
	// Get commands from UDP
//	while(1)
//	{
//		Command * command = newCommand(&p);
//		if(command != nullptr)
//		{
//			if(command->reqSeparateThread())
//			{
//				pthread_t thread;
//				pthread_create(&thread, NULL, Command::commandExeWrapper, static_cast<void *>(command));
//			}
//			else
//			{
//				command->execute();
//				delete command;
//			}
//		}
//	}
}

