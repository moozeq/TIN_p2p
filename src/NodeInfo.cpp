#include "NodeInfo.h"

NodeInfo::~NodeInfo()
{
	// Delete all conditionla variables
	for(auto & pair: nodeFiles)
		delete std::get<2>(pair.second);
}

void NodeInfo::addNewFileEntry(std::string hash, size_t nodeId)
{
	std::unique_lock<std::mutex> uLock(nodeFilesMtx);
	std::condition_variable cond;
	FileInfo info = std::make_tuple(nodeId, 0, new std::condition_variable());
	nodeFiles.insert(std::pair<std::string, FileInfo>(hash, info));
}

void NodeInfo::removeFile(std::string hash)
{
	std::unique_lock<std::mutex> uLock(nodeFilesMtx);
	auto it = nodeFiles.find(hash);
	FileInfo & fInfo = it->second;
	// Wait until there is 0 transfers
	std::get<2>(fInfo)->wait(uLock, [&fInfo]{return std::get<1>(fInfo) == 0;});
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

void NodeInfo::callForEachNode(std::function<void (struct in_addr *)> callback)
{
	std::unique_lock<std::mutex> uLock(nodeMapMtx);
	for(auto & addr : nodeMap)
		callback(&(addr.second));
}

void NodeInfo::callForEachFile(std::function<void (std::string)> callback)
{
	std::unique_lock<std::mutex> uLock(nodeFilesMtx);
	for(auto & file : nodeFiles)
		callback(file.first);
}

size_t NodeInfo::getOwnerId(std::string hash) {
	auto it = nodeFiles.find(hash);
	return std::get<0>(it->second);
}
