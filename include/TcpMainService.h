#ifndef TCPMAINSERVICE_H_
#define TCPMAINSERVICE_H_

#include "Command.h"
#include <cstdlib>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include "Command.h"
#include "MessageFrames.h"
#include "FilesTableReceive.h"

class TcpMainService: public Command
{
public:
	TcpMainService();
	virtual ~TcpMainService();

	void tcpServiceLoop(void);

private:
	Command * getCommand(size_t opcode, int socketFd);

};

#endif /* TCPMAINSERVICE_H_ */
