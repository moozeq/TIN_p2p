#include "NetMainThread.h"
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <pthread.h>
#include <iostream>
#include <errno.h>
#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <sys/uio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdint>
#include <net/if.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "MessageFrames.h"
#include "SendFileTcp.h"
#include "RemoveFile.h"

NodeInfo* NetMainThread::nodeInfo;

void NetMainThread::die(std::string s)
{
    perror(s.c_str());
    pthread_cancel(tcpThread);
    pthread_exit((void*)nullptr);
}

NodeInfo * NetMainThread::getNodeInfo(void){
	return nodeInfo;
}

void NetMainThread::setAndSendInfoMsgUDP(InfoMessage * msg, unsigned _port) {
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
	commonSocketAddrIn.sin_port = htons(_port);

	if (inet_aton(broadcastAddress.c_str() , &commonSocketAddrIn.sin_addr) == 0)
		die("inet_aton");
	if (sendto(commonSocketFd, msg, sizeof(*msg), 0, (struct sockaddr*) &commonSocketAddrIn, slen) < 0)
		die("sendto");
}

void NetMainThread::setInfoMsgUDP(unsigned timeout, unsigned _port) {
	//receive udp socket
	if ((commonSocketFd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		die("socket");
	// zero out the structure
	memset((char *) &commonSocketAddrIn, 0, sizeof(commonSocketAddrIn));
	commonSocketAddrIn.sin_family = AF_INET;
	commonSocketAddrIn.sin_port = htons(_port);
	commonSocketAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);

	int option = 1;
	if (setsockopt(commonSocketFd,SOL_SOCKET,SO_REUSEADDR, &option, sizeof(option)) < 0)
		die(strerror(errno));

	if (timeout > 0) {
		//recv first info about network in x secs, otherwise -> new network
		struct timeval tv;
		tv.tv_sec = timeout;
		tv.tv_usec = 0;
		if (setsockopt(commonSocketFd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0)
			die(strerror(errno));
	}

	//bind socket to port
	if(bind(commonSocketFd , (struct sockaddr*)&commonSocketAddrIn, sizeof(commonSocketAddrIn) ) < 0)
		die("bind");
}

ssize_t NetMainThread::setAndReceiveInfoMsgUDP(unsigned timeout, InfoMessage * msg, unsigned _port) {
	ssize_t recv_len;
	socklen_t slen = sizeof(commonSocketAddrIn);
	setInfoMsgUDP(timeout, _port);
	if ((recv_len = recvfrom(commonSocketFd, msg, sizeof(*msg), 0, (struct sockaddr *) &commonSocketAddrIn, &slen)) < 0 && !timeout)
		die("recvfrom");
	return recv_len;
}

void NetMainThread::receiveNetworkMessages(void) {
	std::cout << "Net Main Thread's started" << std::endl;
	InfoMessage * msg = new InfoMessage();
	size_t nodeCnt = nodeInfo->getNodeCnt();
	socklen_t slen = sizeof(commonSocketAddrIn);
	bool isMe = false;
	setInfoMsgUDP(0, port);
	while (recvfrom(commonSocketFd, msg, sizeof(*msg), 0, (struct sockaddr *) &commonSocketAddrIn, &slen)) {
		switch(msg->opcode) {
		case 100: //new node wants to join
		{
			msg->opcode = 102;
			msg->thirdField = nodeCnt++; //new node's id
			msg->firstField = nodeCnt; //how many nodes
			msg->secondField = nodeInfo->getNodeId();
			NetUtils::sendInfoMsgUDP(msg, commonSocketAddrIn.sin_addr, joinNetworkPort);
			break;
		}
		case 101:
		{
			--nodeCnt;
			if (msg->secondField == getNodeInfo()->getNodeId()) {
				pthread_cancel(tcpThread);
				isMe = true;
			}
			nodeInfo->reconfiguration(msg->firstField, msg->secondField, isMe);
			if (isMe)
				Command::exitCommand(this);
			break;
		}
		case 103:
		{
			pthread_t thread;
			Command * sendFilesTable = new FilesTableSend(commonSocketAddrIn.sin_addr);
			pthread_create(&thread, 0, Command::commandExeWrapper, static_cast<void *>(sendFilesTable));
			pthread_detach(thread);
			break;
		}
		case 203:
		{
			if (msg->secondField == nodeInfo->getNodeCnt()) //proper node
				nodeInfo->reconfiguration(msg->firstField, commonSocketAddrIn.sin_addr);
			else { //wrong node
				nodeCnt = nodeInfo->getNodeCnt(); //reset local nodeCnt
				InfoMessage * faultMsg = new InfoMessage(400);
				NetUtils::sendInfoMsgUDP(faultMsg, commonSocketAddrIn.sin_addr, port);
				delete faultMsg;
			}
			break;
		}
		case 300:
		{
			pthread_t thread;
			Command* removeFile = new RemoveFile(*msg);
			pthread_create(&thread, NULL, Command::commandExeWrapper, static_cast<void *>(removeFile));
			pthread_detach(thread);
			break;
		}
		case 301:
		{
			pthread_t thread;
			Command* sendFileTcp = new SendFileTcp(*msg);
			pthread_create(&thread, NULL, Command::commandExeWrapper, static_cast<void *>(sendFileTcp));
			pthread_detach(thread);
			break;
		}
		case 400:
		{
			std::cout << "Failed when joining to network" << std::endl;
			pthread_cancel(tcpThread);
			delete this;
			exit(1);
		}
		}
	}
	delete msg;
}

void NetMainThread::buildNetwork(void) {
	firstNode = true;
	std::cout << "Didn't receive any response\nStart building new P2P network... ";
	nodeInfo = new NodeInfo();
	nodeInfo->addNewNode(NetUtils::getMyIP());
	InfoMessage * msg = new InfoMessage(400); //if another node's waiting for response
	setAndSendInfoMsgUDP(msg, joinNetworkPort);
	std::cout << "Completed" << std::endl;
	delete msg;
}

void NetMainThread::joinNetwork(InfoMessage * msg) {
	if (msg->opcode == 400) {
		std::cout << "Another node's building P2P network right now" << std::endl;
		Command::exitCommand(this);
	}
	firstNode = false;
	if (msg->opcode == 102) {
		nodeInfo = new NodeInfo(msg->thirdField); //node id, node cnt
		nodeInfo->setNode(nodeInfo->getNodeId(), NetUtils::getMyIP()); //add current node
		nodeInfo->setNode(msg->secondField, commonSocketAddrIn.sin_addr); //add sender node
	}
	if (msg->firstField == nodeInfo->getNodeMapSize()) {
		std::cout << "Network's been constructed" << std::endl;
		return;
	}
	ssize_t recv_len;
	socklen_t slen = sizeof(commonSocketAddrIn);
	while((recv_len = recvfrom(commonSocketFd, msg, sizeof(*msg), 0, (struct sockaddr *) &commonSocketAddrIn, &slen)) > 0) {
		if (msg->opcode == 102) { //msg about network (cnt, sender id, receiver id)
			if (msg->thirdField != nodeInfo->getNodeId())
				die("Network's corrupted");
			else {
				nodeInfo->setNode(msg->secondField, commonSocketAddrIn.sin_addr); //add sender node
				if (msg->firstField == nodeInfo->getNodeMapSize()) {
					std::cout << "Network's been constructed" << std::endl;
					break;
				}
			}
		}
	}
	//now needs to get files from nodes
}

int NetMainThread::init(void)
{
	InfoMessage * msg = new InfoMessage(100);
	setAndSendInfoMsgUDP(msg, port);
	std::cout<<"Sent joining to network request" <<std::endl;
	close(commonSocketFd); //close udp socket for broadcast

	//receive udp socket
	std::cout<<"Waiting for response within " << maxTimeToJoinP2P << " seconds" <<std::endl;
	if (setAndReceiveInfoMsgUDP(maxTimeToJoinP2P, msg, joinNetworkPort) < 0)
		buildNetwork();
	else
		joinNetwork(msg);
	close(commonSocketFd);
	delete msg;
    return 0;
}

void NetMainThread::execute(void)
{
	Command * command;

	if(nodeInfo != nullptr && nodeInfo->isConnected()){
		std::cout<<"Node's already connected to network" << std::endl;
		pthread_exit(NULL);
	}
	init();

	// Create Main tcp listener thread
	command = new TcpMainService();
	pthread_create(&tcpThread, NULL, Command::commandExeWrapper, static_cast<void *>(command));
	pthread_detach(tcpThread);

	if (!firstNode) { //ack of joining
		InfoMessage * msg = new InfoMessage(203, nodeInfo->getNodeCnt(), nodeInfo->getNodeId()); //now is ready to receive files
		setAndSendInfoMsgUDP(msg, port);
		delete msg;
	}
	receiveNetworkMessages();
}

