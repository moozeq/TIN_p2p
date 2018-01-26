#ifndef REMOVEFILE_H_
#define REMOVEFILE_H_

#include "Command.h"
#include "MessageFrames.h"

class RemoveFile: public Command
{
public:
	RemoveFile(InfoMessage _msg) : msg(_msg) {}
	virtual ~RemoveFile() = default;

	void execute(void);
	bool reqSeparateThread(void) {return true;}
private:
	InfoMessage msg;
};

#endif /* REMOVEFILE_H_ */
