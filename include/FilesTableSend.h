#ifndef FILESTABLESEND_H_
#define FILESTABLESEND_H_

#include "Command.h"
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

class FilesTableSend: public Command
{
public:
	FilesTableSend(struct in_addr _targetNodeIP);
	virtual ~FilesTableSend() = default;

	void execute(void);
	bool reqSeparateThread(void) {	return true;	};
private:
	void insertData(std::string hash);
	void sendFilesTableTCP(std::string* stringData);
	std::stringstream fileTableData;
	struct in_addr targetNodeIP;
};

#endif /* FILESTABLESEND_H_ */
