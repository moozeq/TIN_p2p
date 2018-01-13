#ifndef NODEINFO_H_
#define NODEINFO_H_

#include <stdlib.h>
#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <cstdint>
#include <sys/socket.h>
#include <netinet/in.h>

/**
 * 	@brief	Struct NodeInfo which describes each node
 * 	in the network (according to 2nd page of documentation).
 */
class NodeInfo{
public:
	void addNewFile(size_t hash, size_t nodeId);
	void removeFile(size_t hash);
	void addNewNode(size_t nodeId, struct in_addr nodeIP);
	struct in_addr getNodeIP(size_t nodeId);
	NodeInfo(size_t _nodeId = 0, size_t _nodeCnt = 0) :
		nodeId(_nodeId), nodeCnt(_nodeCnt) {}

private:
	size_t nodeId;
	size_t nodeCnt;
	std::map<size_t, struct in_addr> nodeMap;
	std::map<size_t, size_t> nodeFiles;
	std::mutex nodeFilesMtx;
};


#endif /* NODEINFO_H_ */
