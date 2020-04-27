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
#include <net/if.h>
#include <sys/socket.h>

sockets::SocketBase::SocketBase( const std::string &addr, const uint32_t port ) :
    localAddress( addr ),
    localPort( port )
{
   mySocket = 200;
   data     = new packet_t;
}

sockets::SocketBase::~SocketBase()
{
   delete data;
}

std::string sockets::SocketBase::getLocalAddress()
{
   return localAddress;
}

uint32_t sockets::SocketBase::getLocalPort()
{
   return localPort;
}

void sockets::SocketBase::setLocalPort( uint32_t port )
{
   localPort = port;
}

void sockets::SocketBase::setLocalAddressAndPort( const std::string &addr,
                                                  const uint32_t port )
{
   localAddress = addr;
   localPort    = port;
}

sockets::SocketServer::SocketServer( const std::string &addr, const uint32_t port ) :
    SocketBase( addr, port )
{
   mySocket = socket( AF_INET, SOCK_STREAM, 0 );
   if ( 0 > mySocket )
   {
      logging::ERROR( "Could not create socket!", true );
   }
   struct ifreq opt;
   snprintf( opt.ifr_name, sizeof( opt.ifr_name ), "etho0" );
   setsockopt( mySocket, SOL_SOCKET, SO_BINDTODEVICE | SO_REUSEADDR | SO_REUSEPORT, (void *)&opt, sizeof( opt ) );

   struct sockaddr_in serv_addr;
   serv_addr.sin_family      = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port        = htons( localPort );
   int addrlen               = sizeof( serv_addr );
   if ( 0 > bind( mySocket, (struct sockaddr *)&serv_addr, sizeof( serv_addr ) ) )
   {
      logging::ERROR( "Could not bind socket!", true );
   }
}

sockets::SocketServer::~SocketServer()
{
   ::close( mySocket );
}

void sockets::SocketServer::listen( uint8_t connections )
{
   if ( 0 > ::listen( mySocket, connections ) )
   {
      logging::ERROR( "Could not set to listen for connections!", true );
   }
}

int sockets::SocketServer::accept( void )
{
   struct sockaddr_in serv_addr;
   serv_addr.sin_family      = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port        = htons( localPort );
   int addrlen               = sizeof( serv_addr );
   if ( 0 > ( client = ::accept( mySocket, (struct sockaddr *)&serv_addr, (socklen_t *)&addrlen ) ) )
   {
      logging::ERROR( "Encountered error accepting new connection" );
      perror( " " );
   }
   return client;
}

int sockets::SocketServer::send( int client, const char *message )
{
   logging::INFO( "SocketServer::send()", true );
   snprintf( data->header, sizeof( data->header ), "%p:", (void *)this );
   snprintf( data->body, sizeof( data->body ), "%s", message );

   if ( 0 > ::send( client, data, sizeof( *data ), 0 ) )
   {
      logging::ERROR( logging::getErrnoString( "SocketServer::send()" ) );
      return -1;
   }
   else
   {
      return 0;
   }

}

int sockets::SocketServer::read( int client )
{
   logging::INFO( "SocketServer::read()", true );
   if ( 0 > ::read( client, data, sizeof( *data ) ) )
   {
      logging::ERROR( logging::getErrnoString( "Failured at SocketClient::read()"), true );
      return -1;
   }
   else
   {
      buffer = std::string( data->header ) + ":" + std::string( data->body );
      logging::INFO( "Received: " + buffer, true );
      return 0;
   }
}

sockets::SocketClient::SocketClient( const std::string &addr, const uint32_t port ) :
    SocketBase( addr, port )
{
   mySocket = socket( AF_INET, SOCK_STREAM, 0 );
   if ( 0 > mySocket )
   {
      logging::ERROR( "Could not create socket!", true );
   }
}

sockets::SocketClient::~SocketClient()
{
   close( mySocket );
}

int sockets::SocketClient::connect( void )
{
   struct sockaddr_in serv_addr;
   memset( &serv_addr, '0', sizeof( serv_addr ) );
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_port   = htons( localPort );
   if ( 0 >= inet_pton( AF_INET, localAddress.c_str(), &serv_addr.sin_addr ) )
   {
      logging::ERROR( "inet_pton failed!", true );
      perror( " " );
      return -1;
   }
   if ( 0 > ::connect( mySocket, (struct sockaddr *)&serv_addr, sizeof( serv_addr ) ) )
   {
      logging::ERROR( "Encountered error connecting to server!" );
      perror( " " );
   }
   return 1;
}

int sockets::SocketClient::send( const char *message )
{
   logging::INFO( "SocketClient::send()", true );
   snprintf( data->header, sizeof( data->header ), "%p: ", (void *)this );
   snprintf( data->body, sizeof( data->body ), "%s", message );

   if ( 0 > ::send( mySocket, data, sizeof( *data ), 0 ) )
   {
      perror( "SocketClient::send() " );
      return -1;
   }
   else
   {
      return 0;
   }
}

int sockets::SocketClient::echo()
{
   int retVal = send( buffer.c_str() );
   return retVal;
}

int sockets::SocketClient::read()
{
   logging::INFO( "SocketClient::read()", true );
   if ( 0 > ::read( mySocket, data, sizeof( *data ) ) )
   {
      logging::ERROR( logging::getErrnoString( "Failured at SocketClient::read()" ),
                      true );
      return -1;
   }
   else
   {
      buffer = std::string( data->header ) + ":" + std::string( data->body );
      logging::INFO( "Received: " + buffer, true );
      return 0;
   }
}
