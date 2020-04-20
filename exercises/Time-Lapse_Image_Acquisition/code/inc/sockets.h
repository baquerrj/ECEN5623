#ifndef __SOCKETS_H__
#define __SOCKETS_H__

#include <string.h>

#include <exception>
#include <stdexcept>

struct packet_t
{
   char header[ 10 ];
   char body[ 50 ];
};

class SocketBase
{
public:
   SocketBase();
   SocketBase( const SocketBase &sock );
   virtual ~SocketBase();

   virtual std::string getLocalAddress();
   virtual std::string getLocalPort();
   virtual void setLocalPort( std::string port );
   virtual void setLocalAddressAndPort( const std::string &addr,
                                        const std::string port );

public:
   uint8_t sockFd;
   std::string localPort;
   std::string localAddress;

   packet_t *data;
};

class SocketServer : public SocketBase
{
public:
   SocketServer( const std::string &addr, const std::string &port );
   ~SocketServer();
   int accept();
   void listen( uint8_t connections );
   int send( int client );
   int read( int client );

   int client;
   std::string buffer;
};

class SocketClient : public SocketBase
{
public:
   SocketClient( const std::string &addr, const std::string &port );
   ~SocketClient();
   int connect();
   int send( int server );
   int read( int server );

   int server;
   std::string buffer;
};

#endif  // __SOCKETS_H__