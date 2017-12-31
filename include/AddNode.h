/*
 * AddNode.h
 *
 *  Created on: Dec 31, 2017
 *      Author: piotr
 */

#ifndef ADDNODE_H_
#define ADDNODE_H_

#include "Command.h"

class AddNode: public Command
{
public:
	AddNode();
	virtual ~AddNode();

	void execute();
};

#endif /* ADDNODE_H_ */
