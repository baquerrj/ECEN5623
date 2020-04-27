#ifndef __SOCKET_SERVER_H__
#define __SOCKET_SERVER_H__

#include <SocketBase.h>
#include <string.h>

#include <exception>
#include <stdexcept>

class SocketServer : public SocketBase
{
public:
   SocketServer( const std::string& addr, const uint32_t port );
   ~SocketServer();
   int accept();
   void listen( uint8_t connections );
   int send( int client, const char* message );
   int read( int client );

   int client;
   std::string buffer;
};

#endif  //__SOCKET_SERVER_H__
