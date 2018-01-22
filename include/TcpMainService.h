#ifndef TCPMAINSERVICE_H_
#define TCPMAINSERVICE_H_

#include "Command.h"
#include <cstdlib>
#include <pthread.h>
#include <string>
#include <unistd.h>

class TcpMainService: public Command
{
public:
	TcpMainService();
	virtual ~TcpMainService();

	void tcpServiceLoop(void);
};

#endif /* TCPMAINSERVICE_H_ */
