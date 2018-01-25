#include "NodeInfo.h"
#include "NetUtils.h"
#include "Command.h"
#include "SendFileTcp.h"
#include <unistd.h>

NodeInfo::~NodeInfo()
{
	// Delete all conditionla variables
	for(auto & pair: nodeFiles)
		delete std::get<2>(pair.second);
}

void NodeInfo::addNewFileEntry(std::string hash, size_t nodeId)
{
	std::unique_lock<std::mutex> uLock(nodeFilesMtx);
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
	delete std::get<2>(fInfo);	// free heap memory
	nodeFiles.erase(it);
}

void NodeInfo::addNewNode(struct in_addr nodeIP) {
	std::unique_lock<std::mutex> uLock(nodeMapMtx);
	nodeMap.insert(std::pair<size_t,struct in_addr>(nodeCnt, nodeIP));
	std::cout << "Added new node:" << std::endl
				<< "\tNode ID: " << nodeCnt << std::endl
				<< "\tNode IP: " << inet_ntoa(getNodeIP(nodeCnt)) << std::endl;
	++nodeCnt;
}

void NodeInfo::removeNode(size_t nodeId) {
	std::unique_lock<std::mutex> uLock(nodeMapMtx);
	nodeMap.erase(nodeId);
}

struct in_addr NodeInfo::getNodeIP(size_t nodeId) {
	std::unique_lock<std::mutex> uLock(nodeMapMtx);
	std::map<size_t,struct in_addr>::iterator it = nodeMap.find(nodeId);
	if (it == nodeMap.end()) {
		struct in_addr tmp;
		tmp.s_addr = 0;
		return tmp;
	}
	return it->second;
}

void NodeInfo::setNode(size_t nodeId, struct in_addr nodeIP) { //change node IP or create new entry
	std::unique_lock<std::mutex> uLock(nodeMapMtx);
	std::map<size_t,struct in_addr>::iterator it = nodeMap.find(nodeId);
	if (it == nodeMap.end()) {
		nodeMap.insert(std::pair<size_t,struct in_addr>(nodeId, nodeIP));
		return;
	}
	it->second = nodeIP;
}

void NodeInfo::removeFiles(size_t ownerId, std::unique_lock<std::mutex>& uLock) {
	for (auto it = nodeFiles.begin(); it != nodeFiles.end(); ++it) {
		if (std::get<0>(it->second) == ownerId) {
			// Wait until there is 0 transfers on this file
			std::get<2>(it->second)->wait(uLock, [&it]{return std::get<1>(it->second) == 0;});
			unlink(it->first.c_str());
			delete std::get<2>(it->second);	// free heap memory (condition variable)
			nodeFiles.erase(it);
		}
	}
}

// Used only in reconfiguration
void NodeInfo::changeFilesOwner(size_t newOwnerId, size_t oldOwnerId) {
	for (auto it = nodeFiles.begin(); it != nodeFiles.end(); ++it) {
		if (std::get<0>(it->second) == oldOwnerId)
			std::get<0>(it->second) = newOwnerId;
	}
}

void NodeInfo::reconfiguration(size_t newNodeCnt, size_t leavingNodeId, bool isMe) {
	std::unique_lock<std::mutex> uLock(nodeFilesMtx);
	if (newNodeCnt != --nodeCnt) {
		std::cout << "Network's corrupted!" << std::endl;
		return;
	}
	if (nodeCnt == 0 && isMe) { //last node in network
		removeFiles(0, uLock);
		return;
	}
	removeFiles(leavingNodeId, uLock); //remove files from leaving node

	if (newNodeCnt != leavingNodeId) { //not last node
		setNode(leavingNodeId, getNodeIP(newNodeCnt)); //swap nodes
		removeNode(newNodeCnt);
		changeFilesOwner(leavingNodeId, newNodeCnt);

		if (nodeId == newNodeCnt) //this was last added node
			nodeId = leavingNodeId;
	}
	else //last node
		removeNode(newNodeCnt);

	for (auto it = nodeFiles.begin(); it != nodeFiles.end(); ++it) {
		size_t newNodeId = NetUtils::calcNodeId(it->first, this);
		if (newNodeId != nodeId || isMe) { //need to send this file || its leaving
			InfoMessage msg(304, newNodeId, it->first);
			pthread_t thread;
			Command* command = new SendFileTcp(msg, true);
			pthread_create(&thread, NULL, Command::commandExeWrapper, static_cast<void *>(command));
			pthread_join(thread, 0);
			unlink(it->first.c_str());
			delete std::get<2>(it->second);	// free heap memory (condition variable)
			nodeFiles.erase(it);
		}
	}
}

void NodeInfo::reconfiguration(size_t newNodeCnt) {
	std::unique_lock<std::mutex> uLock(nodeFilesMtx);
	if (newNodeCnt != nodeCnt) { //added node before
		std::cout << "Network's corrupted!" << std::endl;
		return;
	}

	for (auto it = nodeFiles.begin(); it != nodeFiles.end(); ++it) {
		size_t newNodeId = NetUtils::calcNodeId(it->first, this);
		if (newNodeId != nodeId) { //need to send this file
			InfoMessage msg(304, newNodeId, it->first);
			pthread_t thread;
			Command* command = new SendFileTcp(msg, true);
			pthread_create(&thread, NULL, Command::commandExeWrapper, static_cast<void *>(command));
			pthread_join(thread, 0);
			unlink(it->first.c_str());
			delete std::get<2>(it->second);	// free heap memory (condition variable)
			nodeFiles.erase(it);
		}
	}
}

void NodeInfo::callForEachNode(std::function<void (struct in_addr *)> callback)
{
	std::unique_lock<std::mutex> uLock(nodeMapMtx);
	for(auto & addr : nodeMap)
		callback(&(addr.second));
}

void NodeInfo::callForEachNode(std::function<void (size_t, struct in_addr *)> callback)
{
	std::unique_lock<std::mutex> uLock(nodeMapMtx);
	for(auto & addr : nodeMap)
		callback(addr.first, &(addr.second));
}

void NodeInfo::callForEachFile(std::function<void (std::string)> callback)
{
	std::unique_lock<std::mutex> uLock(nodeFilesMtx);
	for(auto & file : nodeFiles)
		callback(file.first);
}

void NodeInfo::registerFileTransfer(std::string hash, bool noMutex)
{
	if (!noMutex)
		std::unique_lock<std::mutex> uLock(nodeFilesMtx);
	auto it = nodeFiles.find(hash);
	++std::get<1>(it->second);
}

void NodeInfo::unregisterFileTransfer(std::string hash, bool noMutex)
{
	if (!noMutex)
		std::unique_lock<std::mutex> uLock(nodeFilesMtx);
	auto it = nodeFiles.find(hash);
	--std::get<1>(it->second);
	// Notify if all transfers completed
	if(std::get<1>(it->second) == 0)
		(std::get<2>(it->second))->notify_one();
}

size_t NodeInfo::getOwnerId(std::string hash, bool noMutex) {
	if (!noMutex)
		std::unique_lock<std::mutex> uLock(nodeFilesMtx);
	auto it = nodeFiles.find(hash);
	return std::get<0>(it->second);
}
