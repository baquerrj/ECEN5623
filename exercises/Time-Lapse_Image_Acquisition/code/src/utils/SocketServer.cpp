#include <errno.h>
#include <string.h>
#include <stdlib.h> // atoi
#include <SocketServer.h>

#include <stdio.h> // printf

SocketServer::SocketServer( const SocketBase::socketType_e socketType )
 : SocketBase( socketType )
{
}

SocketServer::~SocketServer()
{
}

int32_t SocketServer::listenSocket( int32_t backLog )
{
   // listen() already has the necessary protections against invalid socket
   // fd, already listening, etc.
   int32_t retVal = listen( mySocketFd, backLog );
   if( 0 != retVal )
   {
      myErrno = errno;
   }
   return retVal;
}

bool SocketServer::acceptSocket( SocketBase &client )
{
   printf( "SocketServer::acceptSocket() entered\n" );
   bool retVal = true;
   struct sockaddr remoteInfo;
   int32_t clientFd = INITIAL_FD;
   if( 0 <= mySocketFd )
   {
      socklen_t nameLen = sizeof( struct sockaddr );
      clientFd = accept( mySocketFd, &remoteInfo, &nameLen );
      if( INITIAL_FD == clientFd )
      {
         myErrno = errno;
         retVal = false;
         printf( "ERROR: SocketServer::acceptSocket() %s\n", strerror( myErrno ) );
      }
      else
      {
         std::string remoteHost;
         int32_t remotePort;
         if( false == getNameInformation( remoteInfo, remoteHost, remotePort ) )
         {
            // If there's a failure, reset the remoteHost and remotePort to their invalid defaults.
            remotePort = -1;
            remoteHost = "";
         }

         retVal &= client.initializeClientData( clientFd, remoteHost, remotePort );
      }
   }
   return retVal;
}

bool SocketServer::setupSocket( const std::string localHost, const int32_t localPort, const uint32_t backlog, const bool nonBlocking )
{
   printf("SocketServer::setupSocket() entered\n" );
   bool retVal = true;
   myPort = localPort;
   myBacklog = backlog;

   std::string tempHost = localHost;
   if( 0 == tempHost.size() )
   {
      tempHost = "localhost";
   }

   if( ( 0 <= localPort ) && ( 0 < backlog ) )
   {
      retVal = SocketBase::setupSocket( tempHost, localPort, nonBlocking );
      if( true == retVal )
      {
         // Perform the bind and listen.
         if( ( 0 != bindSocket() ) ||
             ( 0 != listenSocket( myBacklog ) ) )
         {
            myErrno = errno;
            printf( "binSocket or listenSocket failed: %s\n", strerror(myErrno));
            retVal = false;
         }
      }
   }
   else
   {
      retVal = false;
   }
   printf( "SocketServer::setupSocket() exiting\n" );
   return retVal;
}

bool SocketServer::initializeClientData( const int32_t clientFd, const std::string &host, const int32_t &port )
{
   return false;
}
