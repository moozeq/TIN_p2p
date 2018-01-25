#include "NetUtils.h"
#include "NetMainThread.h"
#include <stdio.h>
#include <string.h> /* for strncpy */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <linux/if_link.h>
#include <iostream>

bool NetUtils::sendInfoMsgUDP(InfoMessage * msg) {
	int commonSocketFd;
	struct sockaddr_in commonSocketAddrIn;
	socklen_t slen = sizeof(commonSocketAddrIn);

	//socket
	if ((commonSocketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		return false;

	int option = 1;
	if (setsockopt(commonSocketFd,SOL_SOCKET,SO_REUSEADDR, &option, sizeof(option)) < 0)
		return false;
	if (setsockopt(commonSocketFd,SOL_SOCKET,SO_BROADCAST, &option, sizeof(option)) < 0)
		return false;

	memset((char *) &commonSocketAddrIn, 0, sizeof(commonSocketAddrIn));
	commonSocketAddrIn.sin_family = AF_INET;
	commonSocketAddrIn.sin_port = htons(NetMainThread::port);
	if (inet_aton(getBroadcastAddress().c_str() , &commonSocketAddrIn.sin_addr) == 0)
		return false;
	if (sendto(commonSocketFd, msg, sizeof(*msg), 0, (struct sockaddr*) &commonSocketAddrIn, slen) < 0)
		return false;

	close(commonSocketFd);
	return true;
}

bool NetUtils::sendInfoMsgUDP(InfoMessage * msg, struct in_addr nodeAddr) {
	int commonSocketFd;
	struct sockaddr_in commonSocketAddrIn;
	socklen_t slen = sizeof(commonSocketAddrIn);

	//socket
	if ((commonSocketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		return false;

	int option = 1;
	if (setsockopt(commonSocketFd,SOL_SOCKET,SO_REUSEADDR, &option, sizeof(option)) < 0)
		return false;

	memset((char *) &commonSocketAddrIn, 0, sizeof(commonSocketAddrIn));
	commonSocketAddrIn.sin_family = AF_INET;
	commonSocketAddrIn.sin_port = htons(NetMainThread::port);
	commonSocketAddrIn.sin_addr = nodeAddr;

	if (sendto(commonSocketFd, msg, sizeof(*msg), 0, (struct sockaddr*) &commonSocketAddrIn, slen) < 0)
		return false;

	close(commonSocketFd);
	return true;
}

bool NetUtils::sendInfoMsgUDP(InfoMessage * msg, size_t nodeId) {
	return NetUtils::sendInfoMsgUDP(msg, NetMainThread::getNodeInfo()->getNodeIP(nodeId));
}

bool NetUtils::sendFileTCP(std::string hash, std::string* stringFile, size_t ownerId, size_t fileNodeId, size_t opcode) {
	int sockfd = 0;
	struct sockaddr_in serv_addr;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return false;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(NetMainThread::port);
	serv_addr.sin_addr = NetMainThread::getNodeInfo()->getNodeIP(fileNodeId);

	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		return false;

	write(sockfd, &opcode, sizeof(size_t));
	write(sockfd, hash.c_str(), hash.size() + 1);
	write(sockfd, &ownerId, sizeof(size_t));
	write(sockfd, stringFile->c_str(), stringFile->size());

	close(sockfd);
	return true;
}


std::string NetUtils::getSelfIpAddress(void)
{
	// Based on: http://man7.org/linux/man-pages/man3/getifaddrs.3.html
	std::string ipAddress("192.168.");

    struct ifaddrs *ifaddr, *ifa;
    int family, s, n;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return std::string("");
    }

    /* Walk through linked list, maintaining head pointer so we
       can free list later */
    for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;

        /* For an AF_INET* interface address, display the address */

        if (family == AF_INET || family == AF_INET6) {
            s = getnameinfo(ifa->ifa_addr,
                    (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                          sizeof(struct sockaddr_in6),
                    host, NI_MAXHOST,
                    NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                return std::string("");
            }

            if(ipAddress.compare(0, 8, host, 8) == 0){
            	ipAddress.assign(host);
            	break;
            }
        }
    }

    freeifaddrs(ifaddr);
	return ipAddress;
}

std::string NetUtils::getSubnetAddress(void)
{
	std::string subnetAddress(getSelfIpAddress());
	subnetAddress = subnetAddress.substr(0, subnetAddress.find_last_of(".") + 1);
	subnetAddress.append("0");
	return subnetAddress;
}

std::string NetUtils::getBroadcastAddress(void)
{
	std::string broadcastAddress(getSelfIpAddress());
	broadcastAddress = broadcastAddress.substr(0, broadcastAddress.find_last_of(".") + 1);
	broadcastAddress.append("255");
	return broadcastAddress;
}

struct in_addr NetUtils::getMyIP() {

	struct sockaddr_in ipAddressStruct;
	if(!inet_pton(AF_INET, getSelfIpAddress().c_str(), &(ipAddressStruct.sin_addr)))
        perror("inet_pton error!");

	return ipAddressStruct.sin_addr;
}
