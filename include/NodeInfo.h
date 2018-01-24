#ifndef NODEINFO_H_
#define NODEINFO_H_

#include <stdlib.h>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <mutex>
#include <cstdint>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <functional>
#include <condition_variable>
#include <tuple>

/**
 * 	@brief	Struct NodeInfo which describes each node
 * 	in the network (according to 2nd page of documentation).
 */
class NodeInfo{
public:
	void addNewFileEntry(std::string hash, size_t nodeId);
	void removeFile(std::string hash);
	void addNewNode(struct in_addr nodeIP);
	void removeNode(size_t nodeId);
	struct in_addr getNodeIP(size_t nodeId);
	size_t getOwnerId(std::string hash);
	void setNodeIdCnt(size_t _nodeId, size_t _nodeCnt) {
		nodeId = _nodeId;
		nodeCnt = _nodeCnt;
	}
	void setNode(size_t nodeId, struct in_addr nodeIP);
	void setNodeId(size_t _nodeId) {nodeId = _nodeId;}
	void setNodeCnt(size_t _nodeCnt) {nodeCnt = _nodeCnt;}
	void setConnected(bool _connected) {	connected = _connected; 	}
	bool isConnected(void)	{	return connected;	}
	size_t getNodeId() {return nodeId;}
	size_t getNodeCnt() {return nodeCnt;}
	size_t getNodeMapSize() {return nodeMap.size();}
	NodeInfo(size_t _nodeId = 0, size_t _nodeCnt = 0) :
		nodeId(_nodeId), nodeCnt(_nodeCnt), connected(false){}
	~NodeInfo();
	void callForEachNode(std::function<void (struct in_addr *)>);
	void callForEachFile(std::function<void (std::string)>);

	// Contains: ownerId, transferCounter, conditionVariable (for transferCounter)
	using FileInfo = std::tuple<size_t, int, std::condition_variable *>;

private:
	size_t nodeId;
	size_t nodeCnt;
	std::map<size_t, struct in_addr> nodeMap;
	std::map<std::string, FileInfo> nodeFiles;	// <file hash, owner id>
	std::mutex nodeMapMtx;
	std::mutex otherMapMtx;
	std::mutex nodeFilesMtx;
	bool connected;
};


#endif /* NODEINFO_H_ */
