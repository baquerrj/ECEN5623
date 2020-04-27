#include <logging.h>
#include <signal.h>
#include <SocketBase.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
/* /sys includes */
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/socket.h>

const static char* LOCALHOST = "127.0.0.1";

SocketBase::SocketBase( const std::string &addr, const uint32_t port ) :
    localAddress( addr ),
    localPort( port )
{
   mySocket = 200;
   data     = new packet_t;
}

SocketBase::~SocketBase()
{
   delete data;
}

std::string SocketBase::getLocalAddress()
{
   return localAddress;
}

uint32_t SocketBase::getLocalPort()
{
   return localPort;
}

void SocketBase::setLocalPort( uint32_t port )
{
   localPort = port;
}

void SocketBase::setLocalAddressAndPort( const std::string &addr,
                                                  const uint32_t port )
{
   localAddress = addr;
   localPort    = port;
}
