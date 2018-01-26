#include "GetFile.h"
#include "NetMainThread.h"
#include "NodeInfo.h"
#include "MessageFrames.h"
#include "NetUtils.h"

using namespace std;

void GetFile::execute(void) {
	if (NetMainThread::getNodeInfo() == nullptr)
		return;
	size_t nodeId = NetUtils::calcNodeId(param, NetMainThread::getNodeInfo());
	size_t myId = NetMainThread::getNodeInfo()->getNodeId();
	InfoMessage* msg = new InfoMessage(301, myId, param);
	NetUtils::sendInfoMsgUDP(msg, nodeId);
	delete msg;
}
