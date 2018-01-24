#ifndef SRC_GETFILE_H_
#define SRC_GETFILE_H_

#include "Command.h"
#include <string>


class GetFile: public Command {
public:
	GetFile(std::string _param) : param(_param) {}
	virtual ~GetFile() {}

	void execute(void);
	bool reqSeparateThread(void) {return true;}
private:
	std::string param;
};

#endif /* SRC_GETFILE_H_ */
