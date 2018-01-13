#include "NetMainThread.h"
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <pthread.h>
#include <iostream>
#include <errno.h>
#include "InfoMessage.h"
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

pthread_t receiver;
struct sockaddr_in si;
int s; //socket
NodeInfo* NetMainThread::nodeInfo;

void die(std::string s)
{
    perror(s.c_str());
    exit(1);
}

void setAndSendInfoMsgUDP(int s, InfoMessage * msg) {
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
	if (sendto(s, msg, sizeof(*msg), 0, (struct sockaddr*) &si, slen) < 0)
		die("sendto");
}

ssize_t setAndReceiveInfoMsgUDP(int s, unsigned timeout, InfoMessage * msg) {
	ssize_t recv_len;
	socklen_t slen = sizeof(si);
	//receive udp socket
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		die("socket");
	// zero out the structure
	memset((char *) &si, 0, sizeof(si));
	si.sin_family = AF_INET;
	si.sin_port = htons(PORT);
	si.sin_addr.s_addr = htonl(INADDR_ANY);

	int option = 1;
	if (setsockopt(s,SOL_SOCKET,SO_REUSEADDR, &option, sizeof(option)) < 0)
		die(strerror(errno));

	if (timeout > 0) {
		//recv first info about network in 5 secs, otherwise -> new network
		struct timeval tv;
		tv.tv_sec = timeout;
		tv.tv_usec = 0;
		if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0)
			die(strerror(errno));
	}

	//bind socket to port
	if(bind(s , (struct sockaddr*)&si, sizeof(si) ) < 0)
		die("bind");
	if ((recv_len = recvfrom(s, msg, sizeof(*msg), 0, (struct sockaddr *) &si, &slen)) < 0 && !timeout)
		die("recvfrom");
	return recv_len;
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

void NetMainThread::receiveNetworkMessages(void) {
	std::cout << "Net Main Thread's waiting for requests..." << std::endl;
	InfoMessage * msg = new InfoMessage();
	while (setAndReceiveInfoMsgUDP(s, 0, msg)) {
		switch(msg->opcode) {
		case 100: //new node wants to join
			msg->opcode = 102;
			msg->firstField = nodeInfo->getNodeCnt();
			msg->secondField = nodeInfo->getNodeId();
			msg->thirdField = nodeInfo->getNodeCnt();
			setAndSendInfoMsgUDP(s, msg);
			break;
		case 101:
		case 103:
		case 203:
			nodeInfo->addNewNode(si.sin_addr);
			break;
		case 300:
		case 301: break;
		}
	}
	delete msg;
}

void NetMainThread::buildNetwork(void) {
	std::cout << "Didn't receive any response, start building new P2P network..." << std::endl;
	nodeInfo = new NodeInfo();
	nodeInfo->addNewNode(getMyIP());
	std::cout << "Added new node:" << std::endl
			<< "\tNode ID: " << 0 << std::endl
			<< "\tNode IP: " << inet_ntoa(nodeInfo->getNodeIP(0)) << std::endl;
	std::cout << "New P2P network created" << std::endl;
}

void NetMainThread::joinNetwork(InfoMessage * msg) {
	if (msg->opcode == 102) {
		nodeInfo = new NodeInfo(msg->thirdField, msg->firstField); //node id, node cnt
		nodeInfo->setNode(nodeInfo->getNodeId(), getMyIP()); //add current node
		nodeInfo->setNode(msg->secondField, si.sin_addr); //add sender node
	}
	ssize_t recv_len;
	socklen_t slen = sizeof(si);
	while((recv_len = recvfrom(s, msg, sizeof(*msg), 0, (struct sockaddr *) &si, &slen)) > 0) {
		if (msg->opcode == 102) { //msg about network (cnt, sender id, receiver id)
			if (msg->firstField != nodeInfo->getNodeCnt() || msg->thirdField != nodeInfo->getNodeId())
				die("Network's corrupted");
			else {
				nodeInfo->setNode(msg->secondField, si.sin_addr); //add sender node
				if (nodeInfo->getNodeCnt() == nodeInfo->getNodeMapSize()) {
					std::cout << "Network's been constructed" << std::endl;
					break;
				}
			}
		} else if (msg->opcode == 101) { //msg about unlinking from network
			nodeInfo->setNode(msg->secondField, getMyIP()); //change unlinking node ip to this node ip
			nodeInfo->setNodeId(msg->secondField); //set this node id to id of unlinking node
			if (nodeInfo->getNodeCnt() != (msg->firstField + 1)) //wrong node cnt
				die("Network's corrupted");
			nodeInfo->setNodeCnt(msg->firstField); //set new node cnt--
			if (nodeInfo->getNodeCnt() == nodeInfo->getNodeMapSize()) {
				std::cout << "Network's been constructed" << std::endl;
				break;
			}
		}
	}
	close(s);
	msg->opcode = 203; //nodeMap built
	setAndSendInfoMsgUDP(s, msg);
	//now needs to get files from nodes
}

int NetMainThread::init(void)
{
	InfoMessage * msg = new InfoMessage(100);
	setAndSendInfoMsgUDP(s, msg);
	std::cout<<"Sent request - joining network, opcode = " << msg->opcode <<std::endl;
	close(s); //close udp socket for broadcast

	//receive udp socket
	std::cout<<"Waiting for response within " << 5 << " seconds" <<std::endl;
	if (setAndReceiveInfoMsgUDP(s, 5, msg) < 0)
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
	receiveNetworkMessages();

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

