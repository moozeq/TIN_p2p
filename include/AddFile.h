#ifndef ADDNODE_H_
#define ADDNODE_H_

#include "Command.h"
#include <string>
#include "NetUtils.h"

class AddFile: public Command
{
public:
	AddFile(std::string _param) : param(_param) {}
	virtual ~AddFile() {}

	void execute(void);
	bool reqSeparateThread(void) {return true;}
private:
	std::string param;
};

#endif /* ADDNODE_H_ */
