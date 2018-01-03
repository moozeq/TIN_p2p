#ifndef NODEINFO_H_
#define NODEINFO_H_

#include <stdlib.h>
#include <string>
#include <map>
#include <vector>
#include <mutex>

/**
 * 	@brief	Struct NodeInfo which describes each node
 * 	in the network (according to 2nd page of documentation).
 */
class NodeInfo{
	size_t node_id;
	size_t node_cnt;
	std::map<size_t, std::string> node_map;

	void addNewFile(size_t hash, size_t nodeId);
	void removeFile(size_t hash);

private:
	std::map<size_t, size_t> node_files;
	std::mutex nodeFilesMtx;
};


#endif /* NODEINFO_H_ */
