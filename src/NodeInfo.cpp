#include "NodeInfo.h"

void NodeInfo::addNewFile(std::string hash, std::string* fileString, size_t ownerId)
{
	std::unique_lock<std::mutex> uLock(nodeFilesMtx);
	nodeFiles.insert(std::pair<std::string, size_t>(hash, ownerId));
	std::ofstream file(hash, std::ios::out | std::ios::binary);
	if (!file) {
		std::cout << "Couldn't add new file" << std::endl;
		return;
	}
	file << *fileString;
}

void NodeInfo::removeFile(std::string hash)
{
	std::unique_lock<std::mutex> uLock(nodeFilesMtx);
	auto it = nodeFiles.find(hash);
	nodeFiles.erase(it);
}

void NodeInfo::addNewNode(struct in_addr nodeIP) {
	nodeMap.insert(std::pair<size_t,struct in_addr>(nodeCnt, nodeIP));
	std::cout << "Added new node:" << std::endl
				<< "\tNode ID: " << nodeCnt << std::endl
				<< "\tNode IP: " << inet_ntoa(getNodeIP(nodeCnt)) << std::endl;
	++nodeCnt;
}

void NodeInfo::removeNode(size_t nodeId) {
	nodeMap.erase(nodeId);
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

void NodeInfo::setNode(size_t nodeId, struct in_addr nodeIP) { //change node IP or create new entry
	std::map<size_t,struct in_addr>::iterator it = nodeMap.find(nodeId);
	if (it == nodeMap.end()) {
		nodeMap.insert(std::pair<size_t,struct in_addr>(nodeId, nodeIP));
		return;
	}
	it->second = nodeIP;
}
