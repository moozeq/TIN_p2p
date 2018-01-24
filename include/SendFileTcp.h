#ifndef SRC_SENDFILETCP_H_
#define SRC_SENDFILETCP_H_
#include "Command.h"
#include "MessageFrames.h"

class SendFileTcp: public Command {
public:
	SendFileTcp(InfoMessage* _msg) : msg(_msg) {}
	virtual ~SendFileTcp() {}

	void execute(void);
	bool reqSeparateThread(void) {return true;}
private:
	InfoMessage* msg;
};

#endif /* SRC_SENDFILETCP_H_ */
