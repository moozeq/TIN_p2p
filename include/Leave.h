#ifndef SRC_LEAVE_H_
#define SRC_LEAVE_H_

#include "Command.h"

class Leave: public Command {
public:
	Leave() {}
	virtual ~Leave() {}

	void execute(void);
	bool reqSeparateThread(void) {return false;}
};

#endif /* SRC_LEAVE_H_ */
