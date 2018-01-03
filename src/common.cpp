#include "common.h"

void * commandExeWrapper(void * command)
{
	Command * cmd = static_cast<Command *>(command);
	cmd->execute();
	delete cmd;
	return 0;
}
