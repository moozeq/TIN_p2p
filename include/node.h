/*
 * node.h
 *
 *  Created on: Dec 30, 2017
 *      Author: piotr
 */

#ifndef NODE_H_
#define NODE_H_

#include <stdlib.h>
#include <string>
#include <map>
#include <vector>

/**
 * 	@brief	Struct NodeInfo which describes each node
 * 	in the network (according to 2nd page of documentation).
 */
struct NodeInfo{
	size_t node_id;
	size_t node_cnt;
	std::map<size_t, std::string> node_map;
	std::vector<std::pair<size_t, size_t>> node_files;
};


#endif /* NODE_H_ */
