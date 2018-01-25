#ifndef NETUTILS_H_
#define NETUTILS_H_
#include "MessageFrames.h"
#include <string>

class NetUtils
{
public:
	static std::string getSelfIpAddress(void);
	static std::string getSubnetAddress(void);
	static std::string getBroadcastAddress(void);
	static bool sendFileTCP(std::string hash, std::string* stringFile, size_t nodeId, size_t fileNodeId, size_t opcode);
	static bool sendInfoMsgUDP(InfoMessage * msg);
	static bool sendInfoMsgUDP(InfoMessage * msg, size_t nodeId);
	static bool sendInfoMsgUDP(InfoMessage * msg, struct in_addr nodeAddr);
	static struct in_addr getMyIP();
};

#endif /* NETUTILS_H_ */
