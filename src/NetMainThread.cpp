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

NodeInfo* NetMainThread::nodeInfo;

void die(std::string s)
{
    perror(s.c_str());
    exit(1);
}

NodeInfo * NetMainThread::getNodeInfo(void){
	return nodeInfo;
}

void NetMainThread::setAndSendInfoMsgUDP(InfoMessage * msg) {
	socklen_t slen = sizeof(commonSocketAddrIn);

	//socket
	if ((commonSocketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		die("socket");

	//broadcast
	int broadcast = 1, option = 1;
	if (setsockopt(commonSocketFd,SOL_SOCKET,SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
		die(strerror(errno));
	if (setsockopt(commonSocketFd,SOL_SOCKET,SO_REUSEADDR, &option, sizeof(option)) < 0)
		die(strerror(errno));

	memset((char *) &commonSocketAddrIn, 0, sizeof(commonSocketAddrIn));
	commonSocketAddrIn.sin_family = AF_INET;
	commonSocketAddrIn.sin_port = htons(port);

	if (inet_aton(broadcastAddress.c_str() , &commonSocketAddrIn.sin_addr) == 0)
		die("inet_aton() failed\n");
	if (sendto(commonSocketFd, msg, sizeof(*msg), 0, (struct sockaddr*) &commonSocketAddrIn, slen) < 0)
		die("sendto");
}

ssize_t NetMainThread::setAndReceiveInfoMsgUDP(unsigned timeout, InfoMessage * msg) {
	ssize_t recv_len;
	socklen_t slen = sizeof(commonSocketAddrIn);
	//receive udp socket
	if ((commonSocketFd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		die("socket");
	// zero out the structure
	memset((char *) &commonSocketAddrIn, 0, sizeof(commonSocketAddrIn));
	commonSocketAddrIn.sin_family = AF_INET;
	commonSocketAddrIn.sin_port = htons(port);
	commonSocketAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);

	int option = 1;
	if (setsockopt(commonSocketFd,SOL_SOCKET,SO_REUSEADDR, &option, sizeof(option)) < 0)
		die(strerror(errno));

	if (timeout > 0) {
		//recv first info about network in 5 secs, otherwise -> new network
		struct timeval tv;
		tv.tv_sec = timeout;
		tv.tv_usec = 0;
		if (setsockopt(commonSocketFd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0)
			die(strerror(errno));
	}

	//bind socket to port
	if(bind(commonSocketFd , (struct sockaddr*)&commonSocketAddrIn, sizeof(commonSocketAddrIn) ) < 0)
		die("bind");
	if ((recv_len = recvfrom(commonSocketFd, msg, sizeof(*msg), 0, (struct sockaddr *) &commonSocketAddrIn, &slen)) < 0 && !timeout)
		die("recvfrom");
	return recv_len;
}

void NetMainThread::receiveNetworkMessages(void) {
	std::cout << "Net Main Thread's waiting for requests..." << std::endl;
	InfoMessage * msg = new InfoMessage();
	while (setAndReceiveInfoMsgUDP(0, msg)) {
		close(commonSocketFd);
		switch(msg->opcode) {
		case 100: //new node wants to join
			msg->opcode = 102;
			msg->firstField = nodeInfo->getNodeCnt();
			msg->secondField = nodeInfo->getNodeId();
			msg->thirdField = nodeInfo->getNodeCnt();
			//struct sockaddr_in siRec;
			//siRec = si;
			setAndSendInfoMsgUDP(msg);
			close(commonSocketFd);
			break;
		case 101:
		case 103:
		case 203:
			nodeInfo->addNewNode(commonSocketAddrIn.sin_addr);
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
	nodeInfo->addNewNode(NetUtils::getMyIP());
	std::cout << "New P2P network created" << std::endl;
}

void NetMainThread::joinNetwork(InfoMessage * msg) {
	if (msg->opcode == 102) {
		nodeInfo = new NodeInfo(msg->thirdField, msg->firstField + 1); //node id, node cnt
		nodeInfo->setNode(nodeInfo->getNodeId(), NetUtils::getMyIP()); //add current node
		nodeInfo->setNode(msg->secondField, commonSocketAddrIn.sin_addr); //add sender node
	}
	ssize_t recv_len;
	socklen_t slen = sizeof(commonSocketAddrIn);
	while((recv_len = recvfrom(commonSocketFd, msg, sizeof(*msg), 0, (struct sockaddr *) &commonSocketAddrIn, &slen)) > 0) {
		if (msg->opcode == 102) { //msg about network (cnt, sender id, receiver id)
			if (msg->firstField + 1 != nodeInfo->getNodeCnt() || msg->thirdField != nodeInfo->getNodeId())
				die("Network's corrupted");
			else {
				nodeInfo->setNode(msg->secondField, commonSocketAddrIn.sin_addr); //add sender node
				if (nodeInfo->getNodeCnt() == nodeInfo->getNodeMapSize()) {
					std::cout << "Network's been constructed" << std::endl;
					break;
				}
			}
		} else if (msg->opcode == 101) { //msg about unlinking from network
			nodeInfo->setNode(msg->secondField, NetUtils::getMyIP()); //change unlinking node ip to this node ip
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
	close(commonSocketFd);
	msg->opcode = 203; //nodeMap built
	setAndSendInfoMsgUDP(msg);
	//now needs to get files from nodes
}

int NetMainThread::init(void)
{
	InfoMessage * msg = new InfoMessage(100);
	setAndSendInfoMsgUDP(msg);
	std::cout<<"Sent request - joining network, opcode = " << msg->opcode <<std::endl;
	close(commonSocketFd); //close udp socket for broadcast

	//receive udp socket
	std::cout<<"Waiting for response within " << 10 << " seconds" <<std::endl;
	if (setAndReceiveInfoMsgUDP(10, msg) < 0)
		buildNetwork();
	else
		joinNetwork(msg);
	delete msg;
	close(commonSocketFd);

    return 0;
}

void NetMainThread::execute(void)
{
	if(getNodeInfo() != nullptr && getNodeInfo()->isConnected())
		pthread_exit(NULL);
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

