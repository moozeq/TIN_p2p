#include "GetFile.h"
#include "NetMainThread.h"
#include "NodeInfo.h"
#include "MessageFrames.h"

using namespace std;

extern size_t calcNodeId(std::string hash);

void GetFile::execute(void) {
	size_t nodeId = NetUtils::calcNodeId(param, NetMainThread::getNodeInfo());
	size_t myId = NetMainThread::getNodeInfo()->getNodeId();
	InfoMessage* msg = new InfoMessage(301, myId, param);
	NetUtils::sendInfoMsgUDP(msg, nodeId);
	delete msg;
}
