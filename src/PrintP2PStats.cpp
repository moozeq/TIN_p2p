#include "PrintP2PStats.h"
#include "NetMainThread.h"

void PrintP2PStats::printNodeInfo(size_t nodeId, struct in_addr * ipAddress)
{
	 std::cout<<"\tNode " << nodeId;
	 std::cout << ": " << inet_ntoa(*ipAddress);
	 if(*ipAddress == NetUtils::getMyIP())
		 std::cout <<" (current node)" << std::endl;
	 else
		 std::cout << std::endl;
}

void PrintP2PStats::execute(void)
{
	if (NetMainThread::getNodeInfo() == nullptr)
		return;
	NodeInfo * nodeInfo = NetMainThread::getNodeInfo();
	std::cout << "\nP2P network statistics: "<< std::endl;
	nodeInfo->callForEachNode(std::bind(&PrintP2PStats::printNodeInfo,
			this, std::placeholders::_1, std::placeholders::_2));
	std::cout << "\tNodes count: " << nodeInfo->getNodeCnt() << std::endl << std::endl;
}
