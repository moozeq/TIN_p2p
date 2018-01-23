#ifndef ADDNODE_H_
#define ADDNODE_H_

#include "Command.h"
#include <string>

class AddFile: public Command
{
public:
	std::string param;
	AddFile(std::string _param) : param(_param) {}
	virtual ~AddFile() {}

	void execute(void);
	bool reqSeparateThread(void) {return true;}
};

#endif /* ADDNODE_H_ */
