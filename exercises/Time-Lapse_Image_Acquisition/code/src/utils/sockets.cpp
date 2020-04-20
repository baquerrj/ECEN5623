#include <logging.h>
#include <signal.h>
#include <sockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
/* /sys includes */
#include <arpa/inet.h>
#include <sys/socket.h>

const char *LOCALHOST      = "127.0.0.1";
const uint32_t DEFAULTPORT = 8080;

SocketBase::SocketBase()
{
   socket       = 100;
   localAddress = LOCALHOST;    //std::string( "127.0.0.1" );
   localPort    = DEFAULTPORT;  //"8080";
   data         = new packet_t;
}

SocketBase::~SocketBase()
{
   // do nothing
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

SocketServer::SocketServer( const std::string &addr, const uint32_t port )
{
   localAddress = addr;
   localPort    = port;
   socket       = socket( AF_INET, SOCK_STREAM, 0 );
   if ( 0 > socket )
   {
      logging::ERROR( "Could not create socket!", true );
   }
   int opt = 1;
   setsockopt( socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof( opt ) );

   struct sockaddr_in serv_addr;
   serv_addr.sin_family      = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port        = htons( localPort );
   int addrlen               = sizeof( serv_addr );
   if ( 0 > bind( socket, (struct sockaddr *)&serv_addr, sizeof( serv_addr ) ) )
   {
      logging::ERROR( "Could not bind socket!", true );
   }
}

SocketServer::~SocketServer()
{
   ::close( socket );
}

void SocketServer::listen( uint8_t connections )
{
   if ( 0 > ::listen( socket, connections ) )
   {
      logging::ERROR( "Could not set to listen for connections!", true );
   }
}

int SocketServer::accept( void )
{
   struct sockaddr_in serv_addr;
   serv_addr.sin_family      = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port        = htons( localPort );
   int addrlen               = sizeof( serv_addr );
   if ( 0 > ( client = ::accept( socket, (struct sockaddr *)&serv_addr, (socklen_t *)&addrlen ) ) )
   {
      logging::ERROR( "Encountered error accepting new connection" );
      perror( " " );
   }
   return client;
}

int SocketServer::send( int client )
{
   logging::INFO( "SocketServer::send()", true );
   sprintf( data->header, "foo" );
   sprintf( data->body, "bar" );

   if ( 0 > ::send( client, data, sizeof( *data ), 0 ) )
   {
      perror( "SocketServer::send() " );
   }
}

int SocketServer::read( int client )
{
   logging::INFO( "SocketServer::read()", true );
   if ( 0 > ::read( client, data, sizeof( *data ) ) )
   {
      logging::ERROR( "Failured at SocketClient::read()", true );
      perror( " " );
   }
   else
   {
      buffer = std::string( data->header ) + ":" + std::string( data->body );
      logging::INFO( "Received: " + buffer, true );
   }
}

SocketClient::SocketClient( const std::string &addr, const uint32_t port )
{
   localAddress = addr;
   localPort    = port;
   socket       = socket( AF_INET, SOCK_STREAM, 0 );
   if ( 0 > socket )
   {
      logging::ERROR( "Could not create socket!", true );
   }
}

SocketClient::~SocketClient()
{
   close( socket );
}

int SocketClient::connect( void )
{
   struct sockaddr_in serv_addr;
   memset( &serv_addr, '0', sizeof( serv_addr ) );
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_port   = htons( localPort );
   if ( 0 >= inet_pton( AF_INET, "127.0.0.1", &serv_addr.sin_addr ) )
   {
      logging::ERROR( "inet_pton failed!", true );
      perror( " " );
      return -1;
   }
   if ( 0 > ::connect( socket, (struct sockaddr *)&serv_addr, sizeof( serv_addr ) ) )
   {
      logging::ERROR( "Encountered error connecting to server!" );
      perror( " " );
   }
   return 1;
}

int SocketClient::send()
{
   logging::INFO( "SocketClient::send()", true );
}

int SocketClient::read()
{
   logging::INFO( "SocketClient::read()", true );
   if ( 0 > ::read( socket, data, sizeof( *data ) ) )
   {
      logging::ERROR( "Failured at SocketClient::read()", true );
      perror( " " );
   }
   else
   {
      buffer = std::string( data->header ) + ":" + std::string( data->body );
      logging::INFO( "Received: " + buffer, true );
   }
}
