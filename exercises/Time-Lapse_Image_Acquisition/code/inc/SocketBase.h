#ifndef __SOCKET_BASE_H__
#define __SOCKET_BASE_H__

#include <string.h>

#include <exception>
#include <stdexcept>

namespace sockets
{
static const char *LOCALHOST      = "127.0.0.1";
static const uint32_t DEFAULTPORT = 8080;
}  // namespace sockets


class SocketBase
{
public:
   struct packet_t
   {
      char header[ 10 ];
      char body[ 50 ];
   };

public:
   SocketBase()
   {
      mySocket     = 100;
      localAddress = sockets::LOCALHOST;
      localPort    = sockets::DEFAULTPORT;
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