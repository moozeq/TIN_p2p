#include "SendFileTcp.h"
#include "NetMainThread.h"
#include <string>

void SendFileTcp::execute(void) {
	size_t ownerId = NetMainThread::getNodeInfo()->getOwnerId(msg.hash);
	NetMainThread::getNodeInfo()->registerFileTransfer(msg.hash);
	std::string fileStr;
	std::ifstream file(msg.hash, std::ios::in | std::ios::binary);
	if (!file) {
		std::cout << "Couldn't open file" << std::endl;
		return;
	}
	file.seekg(0, std::ios::end); //how big is file
	fileStr.reserve(file.tellg());
	file.seekg(0, std::ios::beg);

	fileStr.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()); //copy file to string

	NetUtils::sendFileTCP(msg.hash, &fileStr, ownerId, msg.firstField, msg.opcode);
	NetMainThread::getNodeInfo()->unregisterFileTransfer(msg.hash);
}
