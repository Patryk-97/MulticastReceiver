#include "MulticastReceiver.h"

MulticastReceiver::MulticastReceiver()
{
   this->ipProtocol = IpProtocol::IPV4;
   this->socketId = INVALID_SOCKET;
}

MulticastReceiver::~MulticastReceiver()
{
   this->reset();
}

void MulticastReceiver::reset(void)
{
   this->close();
   this->socketId = INVALID_SOCKET;
}

bool MulticastReceiver::init(IpProtocol ipProtocol)
{
   // locals
   int family = AF_INET;
   int type = SOCK_DGRAM;
   int proto = IPPROTO_UDP;
   bool rV = true;
   sockaddr_in localSocketAddr;
   int localSocketAddrSize = sizeof(localSocketAddr);

   if (ipProtocol == IpProtocol::IPV6)
   {
      family = AF_INET6;
      this->ipProtocol = IpProtocol::IPV6;
   }

   this->socketId = ::socket(family, type, proto);
   if (this->socketId == INVALID_SOCKET)
   {
      rV = false;
   }

   return rV;
}

bool MulticastReceiver::bind(const char* address, const uint16_t port)
{
   // locals
   bool rV = true;
   int reuse = 1;

   if (::setsockopt(this->socketId, SOL_SOCKET, SO_REUSEADDR,
      (char*)&reuse, sizeof(reuse)) < 0)
   {
      rV = false;
   }

   if (rV == true)
   {
      memset((char*)&this->localSock, 0, sizeof(this->localSock));
      this->fillIpProtocolFamily(&this->localSock);
      this->fillPort(port, &this->localSock);
      rV = this->fillNetworkAddressStructure(nullptr, &this->localSock);
   }

   if (rV == true &&
      ::bind(this->socketId, (sockaddr*)&this->localSock, sizeof(this->localSock)) != 0)
   {
      rV = false;
   }

   if (rV == true)
   {
      rV = this->localInterfaceConfig(address);
   }

   return rV; 
}

bool MulticastReceiver::localInterfaceConfig(const char* addressIp)
{
   // locals
   bool rV = true;

   this->group.imr_multiaddr.S_un.S_un_b.s_b1 = 225;
   this->group.imr_multiaddr.S_un.S_un_b.s_b2 = 1;
   this->group.imr_multiaddr.S_un.S_un_b.s_b3 = 1;
   this->group.imr_multiaddr.S_un.S_un_b.s_b4 = 1;

   this->group.imr_interface.s_addr = ::htonl(INADDR_ANY);

   if (rV == true && ::setsockopt(this->socketId, IPPROTO_IP, IP_ADD_MEMBERSHIP,
      (char*)&this->group, sizeof(this->group)) < 0)
   {
      rV = false;
   }

   return rV;
}

int MulticastReceiver::recv(char* recvBuff, int recvBuffSize)
{
   // locals
   int rV = 0;

   rV = ::recv(this->socketId, recvBuff, recvBuffSize, 0);

   return rV;
}

void MulticastReceiver::close()
{
   if (this->socketId != INVALID_SOCKET)
   {
      ::closesocket(this->socketId);
   }
}

void MulticastReceiver::fillAddrInfoCriteria(addrinfo* hints) const
{
   memset(hints, 0, sizeof(*hints));
   hints->ai_socktype = SOCK_DGRAM;
   hints->ai_family = this->ipProtocol == IpProtocol::IPV4 ? AF_INET : AF_INET6;
}

bool MulticastReceiver::fillNetworkAddressStructure(const char* address, sockaddr_in* socketAddr)
{
   // locals
   bool rV = true;
   std::unique_ptr<addrinfo> hints = std::make_unique<addrinfo>();
   addrinfo* res = nullptr;

   if (address == nullptr)
   {
      socketAddr->sin_addr.s_addr = INADDR_ANY;
   }
   else if (address[0] >= '0' && address[0] <= '9')
   {
      if (::inet_pton(AF_INET, address, &socketAddr->sin_addr) != 1)
      {
         rV = false;
      }
   }
   else
   {
      this->fillAddrInfoCriteria(hints.get());
      if (0 != ::getaddrinfo(address, nullptr, hints.get(), &res))
      {
         rV = false;
      }
      else
      {
         socketAddr->sin_addr.S_un = ((sockaddr_in*)(res->ai_addr))->sin_addr.S_un;
         ::freeaddrinfo(res);
      }
   }

   return rV;
}

void MulticastReceiver::fillPort(uint16_t port, sockaddr_in* socketAddr)
{
   socketAddr->sin_port = htons(port);
}

void MulticastReceiver::fillIpProtocolFamily(sockaddr_in* socketAddr)
{
   if (this->ipProtocol == IpProtocol::IPV4)
   {
      socketAddr->sin_family = AF_INET;
   }
   else
   {
      socketAddr->sin_family = AF_INET6;
   }
}