#ifndef SRC_NETMAINTHREAD_H_
#define SRC_NETMAINTHREAD_H_

#include "Command.h"
#include "FileTransfer.h"
#include "NodeInfo.h"
#include "InfoMessage.h"
#include <string>

class NetMainThread: public Command
{
public:
	static NodeInfo * nodeInfo;
	NetMainThread() {}
	virtual ~NetMainThread() {}

	Command * newCommand();
	static NodeInfo* getNodeInfo(void) {return nodeInfo; }
	void execute(void);
	bool reqSeparateThread(void) {return true;}
	int init(void);
	void buildNetwork(void);
	void receiveNetworkMessages(void);
	void parseMsg(InfoMessage * msg);
	void joinNetwork(InfoMessage * req);

};

#endif /* SRC_NETMAINTHREAD_H_ */
