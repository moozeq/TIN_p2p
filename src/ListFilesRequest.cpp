#include "ListFilesRequest.h"

void deleteSelfAndDie(Command * cmd, std::string s)
{
    perror(s.c_str());
    delete cmd;
    pthread_exit((void*)nullptr);
}

void ListFilesRequest::sendInfoMsgUDP(struct in_addr * in_addr)
{
	commonSocketAddrIn.sin_addr = *in_addr;
	if (sendto(commonSocketFd, &requestFilesMessage, sizeof(requestFilesMessage),
			0, (struct sockaddr*) &commonSocketAddrIn, sizeof(commonSocketAddrIn)) < 0)
	{
		deleteSelfAndDie(this, "sendto");
		std::cout<<"List request to x host failed..."<<std::endl;
	}
}

void ListFilesRequest::execute(void)
{
	//socket
	if ((commonSocketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
		deleteSelfAndDie(this, "socket");
		std::cout<<"List request - socket create - failed..."<<std::endl;
	}

	memset((char *) &commonSocketAddrIn, 0, sizeof(commonSocketAddrIn));
	commonSocketAddrIn.sin_family = AF_INET;
	commonSocketAddrIn.sin_port = htons(NetMainThread::port);

	NodeInfo * nInfo = NetMainThread::getNodeInfo();
	nInfo->callForEachNode(std::bind(&ListFilesRequest::sendInfoMsgUDP, this, std::placeholders::_1));
}
