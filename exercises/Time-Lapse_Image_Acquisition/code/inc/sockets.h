#ifndef __SOCKETS_H__
#define __SOCKETS_H__

#include <string.h>

#include <exception>
#include <stdexcept>

namespace sockets
{
extern const char* LOCALHOST;
extern const uint32_t DEFAULTPORT;

struct packet_t
{
   char header[ 10 ];
   char body[ 50 ];
};

class SocketBase
{
public:
   SocketBase()
   {
      mySocket = 100;
      localAddress = LOCALHOST;
      localPort = DEFAULTPORT;
      data = new packet_t;
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
   uint32_t localPort;
   std::string localAddress;

   packet_t *data;
};

class SocketServer : public SocketBase
{
public:
   SocketServer( const std::string &addr, const uint32_t port );
   ~SocketServer();
   int accept();
   void listen( uint8_t connections );
   int send( int client, const char* message );
   int read( int client );

   int client;
   std::string buffer;
};

class SocketClient : public SocketBase
{
public:
   SocketClient( const std::string &addr, const uint32_t port );
   ~SocketClient();
   int connect();
   int send( const char* message );
   int echo();
   int read();

   std::string buffer;
};

}; // namespace sockets

#endif  // __SOCKETS_H__