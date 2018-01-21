#include "NetUtils.h"

#include <stdio.h>
#include <string.h> /* for strncpy */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>

std::string NetUtils::getSelfIpAddress(void)
{
	// Based on: http://www.geekpage.jp/en/programming/linux-network/get-ipaddr.php
	int fd;
	struct ifreq ifr;
	std::string ipAddress;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	/* I want to get an IPv4 IP address */
	ifr.ifr_addr.sa_family = AF_INET;

	/* I want IP address attached to "eth0" */
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);

	ipAddress.assign(inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	return ipAddress;
}

std::string NetUtils::getSubnetAddress(void)
{
	std::string subnetAddress(getSelfIpAddress());
	subnetAddress.replace(subnetAddress.find_last_of("."), 3, "0");
	return subnetAddress;
}

std::string NetUtils::getBroadcastAddress(void)
{
	std::string subnetAddress(getSelfIpAddress());
	subnetAddress.replace(subnetAddress.find_last_of("."), 3, "255");
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
