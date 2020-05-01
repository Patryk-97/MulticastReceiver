#include "MulticastReceiver.h"
#include "WinsockManager.h"

#include <iostream>
#include <memory>

#define RECV_BUFF_SIZE 4096
#define DLL_WINSOCK_VERSION MAKEWORD(2, 2)

int main()
{
   std::unique_ptr<WinsockManager> winsockManager = std::make_unique<WinsockManager>();
   std::unique_ptr<MulticastReceiver> multicastReceiver = nullptr;
   uint16_t port;
   char recvBuff[RECV_BUFF_SIZE];

   if (false == winsockManager->startup(DLL_WINSOCK_VERSION))
   {
      printf("Winsock initialization error\n");
      return -1;
   }

   std::cout << "Enter port: ";
   std::cin >> port;

   multicastReceiver = std::make_unique<MulticastReceiver>();

   if (true == multicastReceiver->init(IpProtocol::IPV4))
   {
      std::cout << "Socket initialized\n";
   }
   else
   {
      std::cout << "Cannot initialiaze a socket\n";
      std::cout << "Error: " << WinsockManager::getErrorMessage() << "\n";
      winsockManager->cleanup();
      return -1;
   }

   if (multicastReceiver->bind("225.1.1.1", port) == false)
   {
      std::cout << "Binding error\n";
      std::cout << "Error: " << WinsockManager::getErrorMessage() << "\n";
      multicastReceiver->close();
      winsockManager->cleanup();
      return -1;
   }

   if (multicastReceiver->recv(recvBuff, RECV_BUFF_SIZE) <= 0)
   {
      std::cout << "Data have not been received\n";
      std::cout << "Error: " << WinsockManager::getErrorMessage() << "\n";
   }
   else
   {
      std::cout << "Received: " << recvBuff << "\n";
   }

   multicastReceiver->close();
   std::cout << "Socket closed" << "\n";
   winsockManager->cleanup();
   std::cin.get();
   return 0;
}