#ifndef ADDNODE_H_
#define ADDNODE_H_

#include "Command.h"

class AddNode: public Command
{
public:
	AddNode() {}
	virtual ~AddNode() {}

	void execute(void);
	bool reqSeparateThread(void) {return true;}
};

#endif /* ADDNODE_H_ */
