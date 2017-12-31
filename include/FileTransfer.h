/*
 * FileTransfer.h
 *
 *  Created on: Dec 31, 2017
 *      Author: piotr
 */

#ifndef FILETRANSFER_H_
#define FILETRANSFER_H_

#include "Command.h"

class FileTransfer: public Command
{
public:
	FileTransfer();
	virtual ~FileTransfer();

	void execute();
};

#endif /* FILETRANSFER_H_ */
