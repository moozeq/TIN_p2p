#ifndef LISTFILESREQUEST_H_
#define LISTFILESREQUEST_H_

#include "Command.h"
#include "MessageFrames.h"
#include "NetMainThread.h"
#include "MessageFrames.h"
#include "NetMainThread.h"
#include "NodeInfo.h"
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <functional>

class ListFilesRequest: public Command
{
public:
	ListFilesRequest() : commonSocketFd(0), commonSocketAddrIn{0, 0, 0}, requestFilesMessage(103) {};
	virtual ~ListFilesRequest() = default;

	void sendInfoMsgUDP(struct in_addr * in_addr);
	void execute(void);
	bool reqSeparateThread(void) {	return true;	};
private:
	int commonSocketFd;
	struct sockaddr_in commonSocketAddrIn;
	InfoMessage requestFilesMessage;
};

#endif /* LISTFILESREQUEST_H_ */
