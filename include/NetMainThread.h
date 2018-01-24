#ifndef SRC_NETMAINTHREAD_H_
#define SRC_NETMAINTHREAD_H_

#include "Command.h"
#include "NodeInfo.h"
#include <string>
#include "NetUtils.h"

#include "MessageFrames.h"
#include "NetUtils.h"
#include "TcpMainService.h"
#include "FilesTableSend.h"

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

	static const unsigned port = 8888;

private:
	std::string broadcastAddress;
	int commonSocketFd;
	struct sockaddr_in commonSocketAddrIn;

	const unsigned maxTimeToJoinP2P = 20;
};

#endif /* SRC_NETMAINTHREAD_H_ */
