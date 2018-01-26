#include "Leave.h"
#include "NetUtils.h"
#include "MessageFrames.h"
#include "NetMainThread.h"

void Leave::execute(void) {
	if (NetMainThread::getNodeInfo() == nullptr)
		return;
	size_t newNodeCnt = NetMainThread::getNodeInfo()->getNodeCnt() - 1;
	size_t leavingNodeId = NetMainThread::getNodeInfo()->getNodeId();
	InfoMessage* msg = new InfoMessage(101, newNodeCnt, leavingNodeId);
	NetUtils::sendInfoMsgUDP(msg);
	delete msg;
}
