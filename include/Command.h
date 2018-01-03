#ifndef COMMAND_H_
#define COMMAND_H_

#include "common.h"

class Command
{
public:
	Command() {}
	virtual ~Command()	{}

	virtual void execute(void) = 0;
	virtual bool reqSeparateThread(void) = 0;
};

#endif /* COMMAND_H_ */
