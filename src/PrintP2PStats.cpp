#include "PrintP2PStats.h"
#include "NetMainThread.h"

void PrintP2PStats::printNodeInfo(size_t nodeId, struct in_addr * ipAddress)
{
	 std::cout<<"Node " << nodeId;
	 if(*ipAddress == NetUtils::getMyIP())
		 std::cout<<" (this node)";
	 std::cout << ", " << inet_ntoa(*ipAddress) << std::endl;
}

void PrintP2PStats::execute(void)
{
	NodeInfo * nodeInfo = NetMainThread::getNodeInfo();
	std::cout << "\nP2P network statistics: "<< std::endl;
	nodeInfo->callForEachNode(std::bind(&PrintP2PStats::printNodeInfo,
			this, std::placeholders::_1, std::placeholders::_2));
	std::cout << "Nodes count: " << nodeInfo->getNodeCnt() << std::endl << std::endl;
}
