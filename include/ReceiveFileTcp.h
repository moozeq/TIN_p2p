#ifndef RECEIVEFILETCP_H_
#define RECEIVEFILETCP_H_

#include "Command.h"
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include "NetMainThread.h"

class ReceiveFileTcp: public Command
{
public:
	explicit ReceiveFileTcp(size_t _opcode, int _socketFd) : opcode(_opcode), socketFd(_socketFd) {};

	virtual ~ReceiveFileTcp() {};

	void updateNodeInfo(std::string hash, size_t fileId);
	void execute(void);
	bool reqSeparateThread(void) {	return true; }

private:
	size_t opcode;
	int socketFd;
};

#endif /* RECEIVEFILETCP_H_ */
