#ifndef SRC_RECEIVETCP_H_
#define SRC_RECEIVETCP_H_

#include "Command.h"

class ReceiveTCP: public Command {
	void execute(void);
	bool reqSeparateThread(void) {return true;}

};

#endif /* SRC_RECEIVETCP_H_ */
