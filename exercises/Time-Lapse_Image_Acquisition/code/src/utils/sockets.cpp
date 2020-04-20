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

SocketBase::SocketBase()
{
   sockFd       = 100;
   localAddress = std::string( "127.0.0.1" );
   localPort    = "8080";
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

std::string SocketBase::getLocalPort()
{
   return localPort;
}

void SocketBase::setLocalPort( std::string port )
{
   localPort = port;
}

void SocketBase::setLocalAddressAndPort( const std::string &addr,
                                         const std::string port )
{
   localAddress = addr;
   localPort    = port;
}

SocketServer::SocketServer( const std::string &addr, const std::string &port )
{
   localAddress = addr;
   localPort    = port;
   sockFd       = socket( AF_INET, SOCK_STREAM, 0 );
   if ( 0 > sockFd )
   {
      logging::ERROR( "Could not create socket!", true );
   }
   int opt = 1;
   setsockopt( sockFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof( opt ) );

   struct sockaddr_in serv_addr;
   serv_addr.sin_family      = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port        = htons( atoi( localPort.c_str() ) );
   int addrlen               = sizeof( serv_addr );
   if ( 0 > bind( sockFd, (struct sockaddr *)&serv_addr, sizeof( serv_addr ) ) )
   {
      logging::ERROR( "Could not bind socket!", true );
   }
}

SocketServer::~SocketServer()
{
   ::close( sockFd );
}

void SocketServer::listen( uint8_t connections )
{
   if ( 0 > ::listen( sockFd, connections ) )
   {
      logging::ERROR( "Could not set to listen for connections!", true );
   }
}

int SocketServer::accept( void )
{
   struct sockaddr_in serv_addr;
   serv_addr.sin_family      = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port        = htons( atoi( localPort.c_str() ) );
   int addrlen               = sizeof( serv_addr );
   if ( 0 > ( client = ::accept( sockFd, (struct sockaddr *)&serv_addr, (socklen_t *)&addrlen ) ) )
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

SocketClient::SocketClient( const std::string &addr, const std::string &port )
{
   localAddress = addr;
   localPort    = port;
   sockFd       = socket( AF_INET, SOCK_STREAM, 0 );
   if ( 0 > sockFd )
   {
      logging::ERROR( "Could not create socket!", true );
   }
}

SocketClient::~SocketClient()
{
   close( sockFd );
}

int SocketClient::connect( void )
{
   struct sockaddr_in serv_addr;
   memset( &serv_addr, '0', sizeof( serv_addr ) );
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_port   = htons( atoi( localPort.c_str() ) );
   if ( 0 >= inet_pton( AF_INET, "127.0.0.1", &serv_addr.sin_addr ) )
   {
      logging::ERROR( "inet_pton failed!", true );
      perror( " " );
      return -1;
   }
   if ( 0 > ::connect( sockFd, (struct sockaddr *)&serv_addr, sizeof( serv_addr ) ) )
   {
      logging::ERROR( "Encountered error connecting to server!" );
      perror( " " );
   }
   return 1;
}

int SocketClient::send( int client )
{
   logging::INFO( "SocketClient::send()", true );
}

int SocketClient::read( int client )
{
   logging::INFO( "SocketClient::read()", true );
   if ( 0 > ::read( sockFd, data, sizeof( *data ) ) )
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
