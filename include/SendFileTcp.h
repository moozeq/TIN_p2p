#ifndef SRC_SENDFILETCP_H_
#define SRC_SENDFILETCP_H_
#include "Command.h"
#include "MessageFrames.h"

class SendFileTcp: public Command {
public:
	SendFileTcp(InfoMessage _msg) : msg(_msg), noMutex(false) {}
	SendFileTcp(InfoMessage _msg, bool _noMutex) : msg(_msg), noMutex(_noMutex) {}
	virtual ~SendFileTcp() {}

	void execute(void);
	bool reqSeparateThread(void) {return true;}
private:
	InfoMessage msg;
	bool noMutex;
};

#endif /* SRC_SENDFILETCP_H_ */
