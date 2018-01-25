#include "RemoveFile.h"
#include "NetMainThread.h"

void RemoveFile::execute(void) {
	NodeInfo * nInfo = NetMainThread::getNodeInfo();
	// Check file owner
	if(msg.firstField == nInfo->getOwnerId(msg.hash))
		nInfo->removeFile(std::string(msg.hash));
}
