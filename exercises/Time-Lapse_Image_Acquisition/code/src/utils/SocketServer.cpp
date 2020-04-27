#include <SocketServer.h>
#include <logging.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
/* /sys includes */
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/socket.h>

SocketServer::SocketServer( const std::string &addr, const uint32_t port ) :
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

SocketServer::~SocketServer()
{
   ::close( mySocket );
}

void SocketServer::listen( uint8_t connections )
{
   if ( 0 > ::listen( mySocket, connections ) )
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
   if ( 0 > ( client = ::accept( mySocket, (struct sockaddr *)&serv_addr, (socklen_t *)&addrlen ) ) )
   {
      logging::ERROR( "Encountered error accepting new connection" );
      perror( " " );
   }
   return client;
}

int SocketServer::send( int client, const char *message )
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