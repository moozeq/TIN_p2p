#include "AddFile.h"
#include "NetMainThread.h"
#include "NodeInfo.h"
#include <iostream>
#include <fstream>
#include <string.h>
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
#define PORT 8888   //The port on which to send data

using namespace std;

size_t calcNodeId(string hash) {
	size_t nodeId;
	for (unsigned i = 0; i < hash.size(); ++i)
		nodeId += (unsigned)hash[i];
	return nodeId % NetMainThread::getNodeInfo()->getNodeCnt();
}

void ptDie(std::string s)
{
    perror(s.c_str());
    pthread_exit((void*)nullptr);
}

void sendFileTCP(string hash, string* stringFile, size_t nodeId) {
	int sockfd = 0;
	struct sockaddr_in serv_addr;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		ptDie("socket");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr = NetMainThread::getNodeInfo()->getNodeIP(nodeId);

	//inet_pton(AF_INET, "192.168.56.102", &serv_addr.sin_addr);

	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		ptDie("connect");

	send(sockfd, (const void*)hash.c_str(), hash.size(), 0);
	send(sockfd, (const void*)nodeId, sizeof(size_t), 0);
	send(sockfd, (const void*)stringFile->c_str(), stringFile->size(), 0);

	close(sockfd);
}

void AddFile::execute(void)
{
	stringstream ss(command), ssMD5;
	string comm, filename, fileStr;
	ss >> comm >> filename;
	ifstream file(filename, ios::in | ios::binary);
	if (!file) {
		cout << "Couldn't open file" << endl;
		return;
	}
	file.seekg(0, ios::end);
	fileStr.reserve(file.tellg());
	file.seekg(0, ios::beg);

	fileStr.assign((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

	unsigned char digest[16];
	MD5((unsigned char*)fileStr.c_str(), fileStr.size(), (unsigned char*)&digest);
	string mdString(32, 'x');
	for(int i = 0; i < 16; i++)
		ssMD5 << hex << (int)digest[i];

	mdString = ssMD5.str();
	cout << "File hash: " << mdString << endl;

	NodeInfo* nodeInfo = NetMainThread::getNodeInfo();
	if (nodeInfo != nullptr) {
		size_t fileNodeId = calcNodeId(mdString);
		 if (nodeInfo->getNodeId() == fileNodeId) //file in adding node
			 NetMainThread::getNodeInfo()->addNewFile(mdString, &fileStr, fileNodeId);
		 else //file in different node
			 sendFileTCP(mdString, &fileStr, fileNodeId);
	}
	else
		cout << "Couldn't add new file" << endl;
	cout << "Successfully added file" << endl;
}
