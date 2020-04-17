#include <sockets.h>

SocketBase::SocketBase()
{
   sockFd = 100;
   localAddress = std::string("Invalid");
   localPort = 0;
}

SocketBase::~SocketBase()
{
   // do nothing
}

std::string SocketBase::getLocalAddress()
{
   return localAddress;
}

uint8_t SocketBase::getLocalPort()
{
   return localPort;
}

void SocketBase::setLocalPort( uint8_t port )
{
   localPort = port;
}

void SocketBase::setLocalAddressAndPort( const std::string &addr,
                                         const uint8_t port )
{
   localAddress = addr;
   localPort = port;
}

SocketReceiver::SocketReceiver()
{
   sockFd = 120;
}

SocketReceiver::~SocketReceiver()
{
   // do nothing
}