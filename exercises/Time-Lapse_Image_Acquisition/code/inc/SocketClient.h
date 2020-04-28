#ifndef __SOCKET_CLIENT_H__
#define __SOCKET_CLIENT_H__

#include <SocketBase.h>
#include <string.h>

#include <exception>
#include <stdexcept>

class SocketClient : public SocketBase
{
public:
   SocketClient( const std::string& addr, const uint32_t port );
   ~SocketClient();
   int connect();
   int send( const char* message );
   int echo();
   int read();
   int receive( char* buffer );

   std::string buffer;
};

#endif  //__SOCKET_CLIENT_H__
