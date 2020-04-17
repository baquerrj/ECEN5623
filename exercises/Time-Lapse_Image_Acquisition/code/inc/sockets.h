#ifndef __SOCKETS_H__
#define __SOCKETS_H__

#include <string.h>

#include <exception>
#include <stdexcept>

class SocketBase
{
public:
   SocketBase();
   SocketBase( const SocketBase &sock );
   virtual ~SocketBase();

   virtual std::string getLocalAddress();
   virtual uint8_t getLocalPort();
   virtual void setLocalPort( uint8_t port );
   virtual void setLocalAddressAndPort( const std::string &addr,
                                        const uint8_t port );

protected:
   uint8_t sockFd;

   uint8_t localPort;
   std::string localAddress;
};

class SocketReceiver : public SocketBase
{
public:
   SocketReceiver();
   ~SocketReceiver();
};
#endif  // __SOCKETS_H__