/*
 * Command.h
 *
 *  Created on: Dec 31, 2017
 *      Author: piotr
 */

#ifndef COMMAND_H_
#define COMMAND_H_

class Command
{
public:
	Command();
	virtual ~Command();

	virtual void execute() = 0;
};

#endif /* COMMAND_H_ */
