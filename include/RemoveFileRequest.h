#ifndef REMOVEFILEREQUEST_H_
#define REMOVEFILEREQUEST_H_

#include "Command.h"
#include "GetFile.h"
#include "NetMainThread.h"
#include "NodeInfo.h"
#include "MessageFrames.h"
#include "NetUtils.h"

class RemoveFileRequest: public Command
{
public:
	RemoveFileRequest(std::string _param) : param(_param) {}
	virtual ~RemoveFileRequest() = default;

	void execute(void);
	bool reqSeparateThread(void) {return false;}
private:
	std::string param;
};

#endif /* REMOVEFILEREQUEST_H_ */
