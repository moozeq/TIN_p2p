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
	std::string ipAddress;

	struct ifaddrs *ifaddr, *ifa;
	int family, s;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1) {
	   perror("getifaddrs");
	   return std::string("");
	}

	/* Walk through linked list, maintaining head pointer so we
	  can free list later */

	ifa = ifaddr;
	if (ifa == NULL || ifa->ifa_addr == NULL)
		return std::string("");

	family = ifa->ifa_addr->sa_family;

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
	}

	freeifaddrs(ifaddr);



	ipAddress.assign(host);
	std::cout<<ipAddress<<std::endl;
	return ipAddress;
}

std::string NetUtils::getSubnetAddress(void)
{
	std::string subnetAddress(getSelfIpAddress());
	subnetAddress.replace(subnetAddress.find_last_of(".") + 1, 3, "0");
	return subnetAddress;
}

std::string NetUtils::getBroadcastAddress(void)
{
	std::string subnetAddress(getSelfIpAddress());
	subnetAddress.replace(subnetAddress.find_last_of(".") + 1, 3, "255");
	return subnetAddress;
}

struct in_addr NetUtils::getMyIP() {
	struct ifreq ifr;
	int fd;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	/* I want to get an IPv4 IP address */
	ifr.ifr_addr.sa_family = AF_INET;

	/* I want IP address attached to "eth0" */
	strncpy(ifr.ifr_name, "enp0s3", IFNAMSIZ-1);

	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);
	return ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
}
