#include "RemoveFileRequest.h"

void RemoveFileRequest::execute()
{
	if (NetMainThread::getNodeInfo() == nullptr)
		return;
	size_t nodeId = NetUtils::calcNodeId(param, NetMainThread::getNodeInfo());
	size_t myId = NetMainThread::getNodeInfo()->getNodeId();
	InfoMessage* msg = new InfoMessage(300, myId, param);
	NetUtils::sendInfoMsgUDP(msg, nodeId);
	delete msg;
}
