#ifndef SRC_NETMAINTHREAD_H_
#define SRC_NETMAINTHREAD_H_

#include "Command.h"
#include "FileTransfer.h"
#include "NodeInfo.h"
#include <string>

// Dummy class - pretend udp packet
struct UdpPacket{
	std::string task;
};

// Dummy class - pretend udp socket
struct UdpSocket{
	UdpPacket * udpPacket;
	UdpPacket getPacket(void){	return *udpPacket;}
};

class NetMainThread: public Command
{
public:
	NetMainThread() : nodeInfo(nullptr) {}
	NetMainThread(NodeInfo * _nodeInfo) : nodeInfo(_nodeInfo) {}
	virtual ~NetMainThread() {}

	Command * newCommand(UdpPacket * p);
	void execute(void);
	bool reqSeparateThread(void) {return true;}

private:
	NodeInfo * nodeInfo;
};

#endif /* SRC_NETMAINTHREAD_H_ */
