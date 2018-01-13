#include "NetMainThread.h"
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <iostream>
#include <errno.h>
#include <InfoMessage.h>
#include <unistd.h>
#include <sys/uio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdint>
#include <net/if.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#define BROAD "127.255.255.255"
#define PORT 8888   //The port on which to send data

struct sockaddr_in si;
int s; //socket
NodeInfo* NetMainThread::nodeInfo;

void die(std::string s)
{
    perror(s.c_str());
    exit(1);
}

struct in_addr getMyIP() {
	 struct ifreq ifr;
	 int fd;

	 fd = socket(AF_INET, SOCK_DGRAM, 0);

	 /* I want to get an IPv4 IP address */
	 ifr.ifr_addr.sa_family = AF_INET;

	 /* I want IP address attached to "eth0" */
	 strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

	 ioctl(fd, SIOCGIFADDR, &ifr);
	 return ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
}

void NetMainThread::buildNetwork(void) {
	std::cout << "Didn't receive any response, start building new P2P network..." << std::endl;
	NetMainThread::nodeInfo = new NodeInfo();
	nodeInfo->addNewNode(0, getMyIP());
	std::cout << "Added new node:\n\tNode ID: 0\n\tNode IP: " << inet_ntoa(nodeInfo->getNodeIP(0)) << std::endl;
	std::cout << "New P2P network created" << std::endl;
}

void NetMainThread::joinNetwork(InfoMessage * req) {

}

int NetMainThread::init(void)
{
    ssize_t recv_len;
	socklen_t slen = sizeof(si);

	//socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        die("socket");

    //broadcast
	int broadcast = 1, option = 1;
	if (setsockopt(s,SOL_SOCKET,SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
		die(strerror(errno));
	if (setsockopt(s,SOL_SOCKET,SO_REUSEADDR, &option, sizeof(option)) < 0)
		die(strerror(errno));

    memset((char *) &si, 0, sizeof(si));
    si.sin_family = AF_INET;
    si.sin_port = htons(PORT);

    if (inet_aton(BROAD , &si.sin_addr) == 0)
        die("inet_aton() failed\n");

	InfoMessage * msg = new InfoMessage(100);
	if (sendto(s, msg, sizeof(*msg), 0, (struct sockaddr*) &si, slen) < 0)
		die("sendto()");
	std::cout<<"Sent request - joining network, opcode = " << msg->opcode <<std::endl;

	close(s); //close udp socket for broadcast

	//receive udp socket
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		die("socket");
	// zero out the structure
	memset((char *) &si, 0, sizeof(si));
	si.sin_family = AF_INET;
	si.sin_port = htons(PORT);
	si.sin_addr.s_addr = htonl(INADDR_ANY);

	//recv first info about network in 5 secs, otherwise -> new network
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0)
		die(strerror(errno));

	//bind socket to port
	if(bind(s , (struct sockaddr*)&si, sizeof(si) ) < 0)
		die("bind");

	std::cout<<"Waiting for response within " << tv.tv_sec << " seconds" <<std::endl;
	if ((recv_len = recvfrom(s, msg, sizeof(*msg), 0, (struct sockaddr *) &si, &slen)) < 0)
		buildNetwork();
	else
		joinNetwork(msg);

	delete msg;
	close(s);
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

