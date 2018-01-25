#include "AddFile.h"
#include "NetMainThread.h"
#include "NodeInfo.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <openssl/md5.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdint>
#include <net/if.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

void AddFile::execute(void)
{
	string fileStr;
	stringstream ssMD5;
	ifstream file(param, ios::in | ios::binary);
	if (!file) {
		cout << "Couldn't open file" << endl;
		return;
	}
	file.seekg(0, ios::end); //how big is file
	fileStr.reserve(file.tellg());
	file.seekg(0, ios::beg);

	fileStr.assign((istreambuf_iterator<char>(file)), istreambuf_iterator<char>()); //copy file to string

	unsigned char digest[16];
	MD5((unsigned char*)fileStr.c_str(), fileStr.size(), (unsigned char*)&digest);

	string mdString;
	static const char hexchars[] = "0123456789abcdef";
	for (int i = 0; i < 16; i++) //convert to string
	{
	    unsigned char b = digest[i];
	    char hex[3];

	    hex[0] = hexchars[b >> 4];
	    hex[1] = hexchars[b & 0xF];
	    hex[2] = 0;

	    mdString.append(hex);
	}

	cout << "File hash: " << mdString << endl;

	//sendFileTCP(mdString, &fileStr, 0);
	NodeInfo* nodeInfo = NetMainThread::getNodeInfo();
	if (nodeInfo != nullptr) {
		size_t fileNodeId = NetUtils::calcNodeId(mdString, NetMainThread::getNodeInfo());
		size_t ownerId = NetMainThread::getNodeInfo()->getNodeId();
		 if (nodeInfo->getNodeId() == fileNodeId){ 			//file in this node
			 std::ofstream file(mdString.c_str(), std::ios::out | std::ios::binary);
			 	if (!file) {
			 		std::cout << "Couldn't add new file" << std::endl;
			 		return;
			 	}
			 file << fileStr;
			 file.close();
			 NetMainThread::getNodeInfo()->addNewFileEntry(mdString, ownerId);
		 }
		 else 												//file in different node
			 NetUtils::sendFileTCP(mdString, &fileStr, ownerId, fileNodeId, 304);
	}
	else {
		cout << "Couldn't add new file" << endl;
		return;
	}
	cout << "Successfully added file" << endl;
}
