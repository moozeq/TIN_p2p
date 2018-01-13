#include "NodeInfo.h"

void NodeInfo::addNewFile(size_t hash, size_t nodeId)
{
	std::unique_lock<std::mutex> uLock(nodeFilesMtx);
	nodeFiles.insert(std::pair<size_t, size_t>(hash, nodeId));
}

void NodeInfo::removeFile(size_t hash)
{
	std::unique_lock<std::mutex> uLock(nodeFilesMtx);
	auto it = nodeFiles.find(hash);
	nodeFiles.erase(it);
}

void NodeInfo::addNewNode(size_t nodeId, struct in_addr nodeIP) {
	nodeMap.insert(std::pair<size_t,struct in_addr>(nodeId, nodeIP));
}

struct in_addr NodeInfo::getNodeIP(size_t nodeId) {
	std::map<size_t,struct in_addr>::iterator it = nodeMap.find(nodeId);
	if (it == nodeMap.end()) {
		struct in_addr tmp;
		tmp.s_addr = 0;
		return tmp;
	}
	return it->second;
}
