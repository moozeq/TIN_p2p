#ifndef FILETRANSFER_H_
#define FILETRANSFER_H_

#include "Command.h"

class FileTransfer: public Command
{
public:
	FileTransfer() {}
	virtual ~FileTransfer() {}

	void execute(void);
	bool reqSeparateThread(void) {return true;}
};

#endif /* FILETRANSFER_H_ */
