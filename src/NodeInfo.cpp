#include "NodeInfo.h"
#include "NetUtils.h"
#include "Command.h"
#include "SendFileTcp.h"
#include <unistd.h>

extern size_t calcNodeId(std::string hash);

void NodeInfo::addNewFileEntry(std::string hash, size_t nodeId)
{
	std::unique_lock<std::mutex> uLock(nodeInfoMtx);
	nodeFiles.insert(std::pair<std::string, size_t>(hash, nodeId));
}

void NodeInfo::removeFile(std::string hash)
{
	std::unique_lock<std::mutex> uLock(nodeInfoMtx);
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

void NodeInfo::removeFiles(size_t ownerId) {
	std::map<std::string, size_t>::iterator it;

	for (it = nodeFiles.begin(); it != nodeFiles.end(); ++it) {
		if (it->second == ownerId) {
			unlink(it->first.c_str());
			nodeFiles.erase(it);
		}
	}
}

void NodeInfo::changeFilesOwner(size_t newOwnerId, size_t oldOwnerId) {
	std::map<std::string, size_t>::iterator it;

	for (it = nodeFiles.begin(); it != nodeFiles.end(); ++it) {
		if (it->second == oldOwnerId)
			it->second = newOwnerId;
	}
}

void NodeInfo::reconfiguration(size_t newNodeCnt, size_t leavingNodeId, bool isMe) {
	std::unique_lock<std::mutex> uLock(nodeInfoMtx);
	if (newNodeCnt != --nodeCnt) {
		std::cout << "Network's corrupted!" << std::endl;
		return;
	}
	if (nodeCnt == 0 && isMe) { //last node in network
		removeFiles(0);
		return;
	}
	removeFiles(leavingNodeId); //remove files from leaving node

	if (newNodeCnt != leavingNodeId) { //not last node
		setNode(leavingNodeId, getNodeIP(newNodeCnt)); //swap nodes
		removeNode(newNodeCnt);
		changeFilesOwner(leavingNodeId, newNodeCnt);

		if (nodeId == newNodeCnt) //this was last added node
			nodeId = leavingNodeId;
	}
	else //last node
		removeNode(newNodeCnt);

	std::map<std::string, size_t>::iterator it;
	for (it = nodeFiles.begin(); it != nodeFiles.end(); ++it) {
		size_t newNodeId = calcNodeId(it->first);
		if (newNodeId != nodeId || isMe) { //need to send this file || its leaving
			InfoMessage* msg = new InfoMessage(304, newNodeId, it->first);
			pthread_t thread;
			Command* command = new SendFileTcp(msg);
			pthread_create(&thread, NULL, Command::commandExeWrapper, static_cast<void *>(command));
			pthread_join(thread, 0);
			unlink(it->first.c_str());
			nodeFiles.erase(it);
			delete msg;
		}
	}
}

void NodeInfo::callForEachNode(std::function<void (struct in_addr *)> callback)
{
	std::unique_lock<std::mutex> uLock(nodeInfoMtx);
	for(auto & addr : nodeMap)
		callback(&(addr.second));
}

void NodeInfo::callForEachFile(std::function<void (std::string)> callback)
{
	std::unique_lock<std::mutex> uLock(nodeInfoMtx);
	for(auto & file : nodeFiles)
		callback(file.first);
}

size_t NodeInfo::getOwnerId(std::string hash) {
	return nodeFiles.find(hash)->second;
}
