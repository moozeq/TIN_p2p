#include "FilesTableSend.h"
#include "NetMainThread.h"
#include "NodeInfo.h"
#include <cstdint>
#include <net/if.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <functional>

FilesTableSend::FilesTableSend(struct in_addr _targetNodeIP)
{
	targetNodeIP = _targetNodeIP;
}

void FilesTableSend::insertData(std::string hash)
{
	fileTableData << hash;
}

void FilesTableSend::execute(void)
{
	NodeInfo * nodeInfo = NetMainThread::getNodeInfo();
	nodeInfo->callForEachFile(std::bind(&FilesTableSend::insertData, this,
			std::placeholders::_1));
	std::string filesData = fileTableData.str();
	sendFilesTableTCP(&filesData);
}

void FilesTableSend::sendFilesTableTCP(std::string* stringData) {
	int sockfd = 0;
	struct sockaddr_in serv_addr;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		Command::printErrAndDie(this, "socket");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(NetMainThread::port);
	serv_addr.sin_addr = targetNodeIP;

	//inet_pton(AF_INET, "192.168.56.101", &serv_addr.sin_addr); //test
	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		Command::printErrAndDie(this, "connect");

	size_t opcode = 302;
	write(sockfd, &opcode, sizeof(size_t));
	write(sockfd, stringData->c_str(), stringData->size());

	close(sockfd);
}
