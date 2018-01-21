#include "NetUtils.h"

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
