#include "GetFile.h"
#include "NetMainThread.h"
#include "NodeInfo.h"
#include "MessageFrames.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdint>
#include <net/if.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

using namespace std;

extern void die(std::string s);

extern size_t calcNodeId(std::string hash);

void setAndSendInfoMsgUDP(InfoMessage * msg, size_t nodeId) {
	int commonSocketFd;
	struct sockaddr_in commonSocketAddrIn;
	socklen_t slen = sizeof(commonSocketAddrIn);

	//socket
	if ((commonSocketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		die("socket");

	int option = 1;
	if (setsockopt(commonSocketFd,SOL_SOCKET,SO_REUSEADDR, &option, sizeof(option)) < 0)
		die(strerror(errno));

	memset((char *) &commonSocketAddrIn, 0, sizeof(commonSocketAddrIn));
	commonSocketAddrIn.sin_family = AF_INET;
	commonSocketAddrIn.sin_port = htons(NetMainThread::port);
	commonSocketAddrIn.sin_addr = NetMainThread::getNodeInfo()->getNodeIP(nodeId);

	if (sendto(commonSocketFd, msg, sizeof(*msg), 0, (struct sockaddr*) &commonSocketAddrIn, slen) < 0)
		die("sendto");
	close(commonSocketFd);
}

void GetFile::execute(void) {
	size_t nodeId = calcNodeId(param);
	size_t myId = NetMainThread::getNodeInfo()->getNodeId();
	InfoMessage* msg = new InfoMessage(301, myId, param);
	setAndSendInfoMsgUDP(msg, nodeId);
	delete msg;
}
