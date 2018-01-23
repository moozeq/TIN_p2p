#ifndef TCPMAINSERVICE_H_
#define TCPMAINSERVICE_H_

#include "Command.h"
#include <cstdlib>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include "MessageFrames.h"
#include "FilesTableReceive.h"
#include "ReceiveFileTcp.h"

class TcpMainService: public Command
{
public:
	TcpMainService() = default;
	virtual ~TcpMainService() = default;

	void execute(void);
	bool reqSeparateThread(void) {	return true; };

private:
	Command * getCommand(size_t opcode, int socketFd);
	void tcpServiceLoop(void);
};

#endif /* TCPMAINSERVICE_H_ */
