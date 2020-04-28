#include <SocketClient.h>
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


SocketClient::SocketClient( const std::string &addr, const uint32_t port ) :
    SocketBase( addr, port )
{
   mySocket = socket( AF_INET, SOCK_STREAM, 0 );
   if ( 0 > mySocket )
   {
      logging::ERROR( "Could not create socket!", true );
   }
}

SocketClient::~SocketClient()
{
   close( mySocket );
}

int SocketClient::connect( void )
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

int SocketClient::send( const char *message )
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

int SocketClient::echo()
{
   int retVal = send( buffer.c_str() );
   return retVal;
}

int SocketClient::read()
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

int SocketClient::receive( char* buffer )
{
   int rc = recv( mySocket, buffer, 921800, 0);
   return rc;
}