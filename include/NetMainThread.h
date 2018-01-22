#ifndef SRC_NETMAINTHREAD_H_
#define SRC_NETMAINTHREAD_H_

#include "Command.h"
#include "FileTransfer.h"
#include "NodeInfo.h"
#include <string>

#include "MessageFrames.h"
#include "NetUtils.h"

class NetMainThread: public Command
{
public:
	static NodeInfo * nodeInfo;
	NetMainThread() : broadcastAddress(NetUtils::getBroadcastAddress()), commonSocketFd(0) {}
	virtual ~NetMainThread() {}

	static NodeInfo * getNodeInfo(void);
	void execute(void);
	bool reqSeparateThread(void) {return true;}
	int init(void);
	void buildNetwork(void);
	void setAndSendInfoMsgUDP(InfoMessage * msg);
	ssize_t setAndReceiveInfoMsgUDP(unsigned timeout, InfoMessage * msg);
	void receiveNetworkMessages(void);
	void parseMsg(InfoMessage * msg);
	void joinNetwork(InfoMessage * req);

private:
	std::string broadcastAddress;
	const unsigned port = 8888;
	int commonSocketFd;
	struct sockaddr_in commonSocketAddrIn;
};

#endif /* SRC_NETMAINTHREAD_H_ */
