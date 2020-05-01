#pragma once

#ifndef __MULTICAST_RECEIVER_H__
#define __MULTICAST_RECEIVER_H__

#include "IpProtocol.h"

#include <Winsock2.h>
#include <string>
#include <stdint.h>
#include <memory>
#include <Ws2tcpip.h>

class MulticastReceiver
{
public:
   MulticastReceiver();
   ~MulticastReceiver();
   void reset(void);
   bool init(IpProtocol ipProtocol);
   bool bind(const char* address, const uint16_t port);
   bool localInterfaceConfig(const char* addressIp);
   int recv(char* recvBuff, int recvBuffSize);
   void close(void);

private:
   void fillAddrInfoCriteria(addrinfo* hints) const;
   bool fillNetworkAddressStructure(const char* address, sockaddr_in* socketAddr);
   void fillPort(uint16_t port, sockaddr_in* socketAddr);
   void fillIpProtocolFamily(sockaddr_in* socketAddr);

private:
   SOCKET socketId;
   sockaddr_in localSock;
   ip_mreq group;
   IpProtocol ipProtocol;
};

#endif // __MULTICAST_RECEIVER_H__