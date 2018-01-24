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


void GetFile::execute(void) {
	size_t nodeId = calcNodeId(param);
	size_t myId = NetMainThread::getNodeInfo()->getNodeId();
	InfoMessage* msg = new InfoMessage(301, myId, param);
	NetUtils::sendInfoMsgUDP(msg, nodeId);
	delete msg;
}
