#ifndef FILESTABLERECEIVE_H_
#define FILESTABLERECEIVE_H_

#include "Command.h"
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>

class FilesTableReceive: public Command
{
public:
	explicit FilesTableReceive(size_t _opcode, int _socketFd) : opcode(_opcode), socketFd(_socketFd) {};
	virtual ~FilesTableReceive() {};

	void execute(void);
	bool reqSeparateThread(void) {	return true; }
private:
	size_t opcode;
	int socketFd;
};

#endif /* FILESTABLERECEIVE_H_ */
