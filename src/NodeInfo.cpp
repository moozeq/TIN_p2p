#include "NodeInfo.h"

void NodeInfo::addNewFile(size_t hash, size_t nodeId)
{
	std::unique_lock<std::mutex> uLock(nodeFilesMtx);
	node_files.insert(std::pair<size_t, size_t>(hash, nodeId));
}

void NodeInfo::removeFile(size_t hash)
{
	std::unique_lock<std::mutex> uLock(nodeFilesMtx);
	auto it = node_files.find(hash);
	node_files.erase(it);
}
