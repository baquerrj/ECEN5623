#ifndef __SOCKET_BASE_H__
#define __SOCKET_BASE_H__

#include <string.h>

#include <exception>
#include <stdexcept>

class SocketBase
{
public:
   struct packet_t
   {
      char header[ 10 ];
      char body[ 50 ];
   };

   static const char* LOCALHOST;
   static const uint32_t DEFAULTPORT = 8080;

public:
   SocketBase()
   {
      mySocket     = 100;
      localAddress = LOCALHOST;
      localPort    = DEFAULTPORT;
      data         = new packet_t;
   }

   SocketBase( const std::string &addr, const uint32_t port );
   SocketBase( const SocketBase &sock );
   virtual ~SocketBase();

   virtual std::string getLocalAddress();
   virtual uint32_t getLocalPort();
   virtual void setLocalPort( uint32_t port );
   virtual void setLocalAddressAndPort( const std::string &addr,
                                        const uint32_t port );

public:
   uint8_t mySocket;
   std::string localAddress;
   uint32_t localPort;

   packet_t *data;
};

#endif  // __SOCKET_BASE_H__