#include <errno.h>
#include <stdlib.h> // atoi
#include <string.h> // memset
#include <stdio.h>  // sprintf
#include <SocketClient.h>

SocketClient::SocketClient( const SocketBase::socketType_e socketType )
 : SocketBase( socketType )
{
}

SocketClient::~SocketClient()
{
}

bool SocketClient::connectSocket()
{
   // connect() already has the necessary protections against invalid socket
   // fd, already connected, etc.
   int32_t retVal = connect( mySocketFd, myHostInfo.ai_addr, myHostInfo.ai_addrlen );
   if( 0 != retVal )
   {
      if( EINPROGRESS == errno )
      {
         fd_set fdSet;
         struct timeval timeout;
         timeout.tv_sec = 5;
         timeout.tv_usec = 0;

         FD_ZERO( &fdSet );
         FD_SET( mySocketFd, &fdSet );
         // First argument is the highest FD value in the set, + 1.
         // Since there's exactly one FD in there, mySocketFd, the value
         // becomes mySocketFd + 1.
         retVal = select( mySocketFd + 1, NULL, &fdSet, NULL, &timeout );
         if( -1 == retVal )
         {
            // ERROR -- select failed.
            myErrno = errno;
         }
         // select should return 1, meaning that all 1 of the file descriptors
         // tripped.
         else if( 1 == retVal )
         {
            retVal = 0;
            // success
         }
         else
         {
            // error - took too long to resolve.
            retVal = -1;
         }
      }
      else
      {
         myErrno = errno;
      }
   }
   return ( 0 == retVal );
}

bool SocketClient::lockUdpToRemoteSide( const std::string remoteHost, const int32_t remotePort )
{
   int32_t retVal = -1;
   if( ( UDP_SOCKET == myHostInfo.ai_socktype ) && ( 0 < remoteHost.size() ) && ( 0 <= remotePort ) )
   {
      struct addrinfo remoteInfo;
      bool status = getAddressInformation( remoteInfo, remoteHost, remotePort );
      if( true == status )
      {
         retVal = connect( mySocketFd, remoteInfo.ai_addr, remoteInfo.ai_addrlen );
         if( 0 != retVal )
         {
            myErrno = retVal;
         }
      }
      else
      {
         myErrno = retVal;
      }
   }
   return ( 0 == retVal );
}

bool SocketClient::setupSocket( const std::string remoteHost,
                                const int32_t remotePort,
                                const bool nonBlocking,
                                const bool bindUdp )
{
   bool retVal = true;

   if( UDP_SOCKET == myHostInfo.ai_socktype )
   {
      std::string tempHost = remoteHost;
      if( 0 == tempHost.size() )
      {
         // locaHost isn't specified, so use 'localhost' as the address for
         // binding.
         tempHost = "localhost";
      }

      // tempHost is now correctly setup.
      // Only bind if the local port is also specified.
      if( DEFAULT_PORT < remotePort )
      {
         // Bind against specified host.
         retVal &= SocketBase::setupSocket( tempHost, remotePort, nonBlocking );
         if(( true == retVal ) && bindUdp )
         {
            retVal &= ( 0 == bindSocket() );
         }
      }
      else
      {
         // A socket is needed, but don't care about what service/port it tries to associate with.
         retVal &= SocketBase::setupSocket( tempHost, 0, nonBlocking );
      }
   }
   // TCP socket -- no binding needed or desired, but connect is required.
   else
   {
      if( ( 0 < remoteHost.size() ) && ( DEFAULT_PORT < remotePort ) )
      {
         retVal &= SocketBase::setupSocket( remoteHost, remotePort, nonBlocking );
         if( true == retVal )
         {
            retVal &= connectSocket();
         }
      }
      else
      {
         // remoteHost isn't specified.  Return failure.
         retVal = false;
      }
   }
   return retVal;
}
