// United Launch Alliance (ULA) Proprietary Information/Export Controlled Information//
/********************************************************************************
   ULA PROPRIETARY INFORMATION
   Copyright (c) 2007 - 2007 United Launch Alliance, LLC.
   Unpublished work.  All rights reserved.
********************************************************************************/
#include "SocketBase.h"

#include <string>
#include <string.h> // memset & memcpy
#include <unistd.h>
#include <stdlib.h> // atoi
#include <fcntl.h> // fcntl -- setting up non-blocking socket
#include <errno.h>
#include <stdio.h> // sprintf
#include <netinet/tcp.h> // setsocketopt values
#include <netinet/in.h>  // setsocketopt values

SocketBase::SocketBase( const SocketBase::socketType_e socketType )
 : mySocketFd( INITIAL_FD ),
   myRemotePort( DEFAULT_PORT ),
   sendFlags( 0 )
{
   myHostInfo.ai_socktype = socketType;
}

SocketBase::~SocketBase()
{
   // Whether close was successful or not doesn't really matter right now.
   closeSocket();
}

bool SocketBase::closeSocket()
{
   bool retVal = false;
   // If the socket fd is valid
   if( 0 <= mySocketFd )
   {
      //! @todo figure out a better way to distinctly report the failure of shutdown/close.

      // Shutting down.  Don't really care about close's return value.
      int32_t status = shutdown( mySocketFd, SHUT_RDWR );
      if( 0 == status )
      {
         status = close( mySocketFd );
         if( 0 == status )
         {
            retVal = true;
            mySocketFd = CLOSING_FD;
         }
         else
         {
            myErrno = errno;
         }
      }
      else
      {
         myErrno = errno;
      }
   }
   return retVal;
}

bool SocketBase::setupSocket( const std::string host, const int32_t port, const bool nonBlocking )
{
   bool retVal = true;
   int32_t status;

   if( INITIAL_FD == mySocketFd )
   {
      struct addrinfo *p_localInfo = NULL;

      if( 0 < host.size() )
      {
         struct addrinfo hints;
         char portBuffer[ 10 ];
         sprintf( portBuffer, "%d", port );
         hints.ai_family    = AF_INET; // IPv4.  For IPv6, use AF_UNSPEC
         hints.ai_socktype  = myHostInfo.ai_socktype;
         hints.ai_flags     = AI_PASSIVE;    // For wildcard IP address
         hints.ai_protocol  = 0;
         hints.ai_canonname = NULL;
         hints.ai_addr      = NULL;
         hints.ai_next      = NULL;
         status = getaddrinfo( host.c_str(), portBuffer, &hints, &p_localInfo );
         if( 0 != status )
         {
            myErrno = status;
            retVal = false;
         }
      }

      if( true == retVal )
      {
         for( struct addrinfo *p_result = p_localInfo; p_result != NULL; p_result = p_result->ai_next )
         {
            mySocketFd = socket( p_result->ai_family, p_result->ai_socktype, p_result->ai_protocol );
            if( INITIAL_FD != mySocketFd )
            {
               if( true == nonBlocking )
               {
                  status = fcntl( mySocketFd, F_SETFL, O_NONBLOCK );
                  if( -1 == status )
                  {
                     myErrno = errno;
                     retVal = false;
                  }
               }
               // Enable address reuse in the event of a failure where the socket is not
               // properly closed (e.g. segfault).  Note that this doesn't allow multiple
               // listen/bind calls on the same port.  For the reuse address option to
               // work, the socket must be inactive.  But we can restart the server application
               // immediately, rather than waiting 2-4 minutes for the socket's TIME_WAIT
               // period to expire (the TIME_WAIT period exists to prevent 'old' socket
               // messages from coming through and confusing the issue.
               //
               // Do NOT run this in a Windows environment.
               if( -1 != status )
               {
                  int32_t on = 1;
                  status = setsockopt( mySocketFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof( on ) );
                  if( 0 != status )
                  {
                     myErrno = errno;
                     retVal = false;
                     // Should I close the socket and return a failure?
                  }
               }
               break;
            }
         }

         if( INITIAL_FD != mySocketFd )
         {
            myErrno = errno;
            retVal = false;
         }

         // Copy the hard earned socket data into my instance variable.
         memcpy( &myHostInfo, p_localInfo, sizeof( struct addrinfo ) );
         freeaddrinfo( p_localInfo );
      }

      retVal = INITIAL_FD != mySocketFd;

   }
   return retVal;
}

int32_t SocketBase::setTcpNodelay( bool enable )
{
   int32_t status = -1;
   if( INITIAL_FD != mySocketFd )
   {
      int32_t on = enable ? 1 : 0;
      status = setsockopt( mySocketFd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof( on ) );
      if ( 0 != status )
      {
         // Set failed; set myErrno
         status = errno;
         myErrno = status;
      }
   }
   else
   {
      // Set myErrno to bad file descriptor error code
      status = EBADF;
      myErrno = status;
   }
   return status;
}

int32_t SocketBase::setReceiveTimeout( double timeout )
{
   int32_t status = -1;
   if( INITIAL_FD != mySocketFd )
   {
      // Linux version uses timeval struct (Windows version uses DWORD in ms)
      // The following assumes Linux
      struct timeval tv;
      tv.tv_sec = ( uint64_t )timeout;
      tv.tv_usec = ( timeout - tv.tv_sec ) * 1000 * 1000;
      status = setsockopt( mySocketFd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof( tv ) );
      if ( 0 != status )
      {
         // Set failed; set myErrno
         status = errno;
         myErrno = status;
      }
   }
   else
   {
      // Set myErrno to bad file descriptor error code
      status = EBADF;
      myErrno = status;
   }
   return status;
}

int32_t SocketBase::bindSocket()
{
   // bind() already has the necessary protections against invalid socket
   // fd, already bound, etc.
   int32_t retVal = bind( mySocketFd, myHostInfo.ai_addr, myHostInfo.ai_addrlen );
   if( 0 != retVal )
   {
      myErrno = errno;
   }
   return retVal;
}

int32_t SocketBase::receive( void *p_data, const int32_t &maxMessageLength, std::string *p_remoteHost, int32_t *p_remotePort )
{
   int32_t   byteCount = 0;
   char     *p_buffer  = ( char * )p_data;
   socklen_t nameLen   = sizeof( struct sockaddr );
   struct sockaddr *p_remoteInfo = NULL;


   // 0  typically means the connection is closed.
   // Hopefully, this while loop will only execute once 99% of the time.
   if( ( NULL != p_data ) && ( 0 <= mySocketFd ) )
   {
      // Attempt to read the entire message.
      byteCount = recvfrom( mySocketFd,
                            p_buffer,
                            maxMessageLength,
                            0,
                            p_remoteInfo,
                            ( NULL == p_remoteInfo ) ? NULL : &nameLen );

      if( -1 == byteCount )
      {
         // An error occurred.  Capture the errno value and return.
         // User must know to check the errno value.
         // Remember, non-blocking is also handled as an error with
         // EAGAIN or EWOULDBLOCK.
         myErrno = errno;
      }

      if( ( NULL != p_remoteHost ) && ( NULL != p_remotePort ) )
      {
         // Caller would like some info about the sender.
         getNameInformation( *p_remoteInfo, *p_remoteHost, *p_remotePort );
      }
   }
   return byteCount;
}

int32_t SocketBase::send( const void *p_data, const int32_t &numBytesToSend, const std::string *p_remoteHost, const int32_t *p_remotePort )
{
   int32_t   bytesLeft      = numBytesToSend;
   uint32_t  bytesProcessed = 0;
   int32_t   retVal         = 0;
   // Used to translate the host/port to socket-usable information.
   struct addrinfo info;

   // If the address data is setup, this pointer will be assigned to info's address data.
   struct sockaddr *p_recipient = NULL;

   // Use buffer internally so I can manipulate the pointer
   // without worrying about the original message pointer.
   char *p_buffer = ( char * )p_data;

   // If the caller specified a remote host & port, try using that information.
   // This is used ONLY for a UDP socket.  TCP sockets completely ignore these fields.
   if( ( NULL != p_remoteHost ) && ( NULL != p_remotePort ) )
   {
      // Set the recipient information to all 0's.
      memset( &info, 0, sizeof( struct addrinfo ) );

      retVal = getAddressInformation( info, *p_remoteHost, *p_remotePort );
      p_recipient = info.ai_addr;
   }

   if( ( NULL != p_data ) && ( 0 <= mySocketFd ) )
   {
      do
      {
         // Continue to try and send the rest of the message if the entire thing
         // wasn't sent yet.
         retVal = sendto( mySocketFd, p_buffer, bytesLeft, sendFlags, p_recipient, ( NULL == p_recipient ) ? 0 : sizeof( struct sockaddr_in ) );

         if( -1 == retVal )
         {
            // An error occurred.  Capture the errno value and return.
            // User must know to check the errno value.
            myErrno = errno;
            break;
         }

         bytesProcessed += retVal;
         bytesLeft      -= retVal;
         if( bytesLeft <= 0 )
         {
            // We have a complete message, return.
            return bytesProcessed;
         }
         else
         {
            // Keep sending.
            // Update the buffer pointer to be incremented by the appropriate
            // amount based on the number of bytes transmitted.
            //@todo if flags has MSG_DONTWAIT then should be abort this write loop?
            p_buffer    = ( char * )( ( uint64_t )p_data + bytesProcessed );
         }
      } while( 0 < bytesLeft );
   }
   return bytesProcessed;
}

bool SocketBase::getAddressInformation( struct addrinfo &info, const std::string &host, const int32_t &port )
{
   bool retVal = false;
   struct addrinfo *p_localInfo = NULL;

   if( 0 < host.size() )
   {
      struct addrinfo hints;
      char portBuffer[ 10 ];
      sprintf( portBuffer, "%d", port );
      hints.ai_family    = AF_INET; // IPv4.  For IPv6, use AF_UNSPEC
      hints.ai_socktype  = myHostInfo.ai_socktype;
      hints.ai_flags     = AI_PASSIVE;    /* For wildcard IP address */
      hints.ai_protocol  = 0;
      hints.ai_canonname = NULL;
      hints.ai_addr      = NULL;
      hints.ai_next      = NULL;
      int32_t status = getaddrinfo( host.c_str(), portBuffer, &hints, &p_localInfo );
      if( 0 != status )
      {
         myErrno = status;
      }
      else
      {
         memcpy( &info, p_localInfo, sizeof( struct addrinfo ) );
         freeaddrinfo( p_localInfo );
      }
      retVal = ( 0 == status );
   }
   return retVal;
}

bool SocketBase::getNameInformation( const struct sockaddr &info, std::string &host, int32_t &port )
{
   bool retVal = false;
   // Caller would like some info about the sender.
   char tempHost[ NI_MAXHOST ];
   char tempService[ NI_MAXSERV ];

   int32_t status = getnameinfo( &info,
                         sizeof( struct sockaddr ),
                         tempHost,
                         NI_MAXHOST,
                         tempService,
                         NI_MAXSERV,
                         NI_NUMERICSERV );

   if( 0 == status )
   {
      host = tempHost;
      port = atoi( tempService );
      retVal = true;
   }
   return retVal;
}

bool SocketBase::initializeClientData( const int32_t clientFd, const std::string &host, const int32_t &port )
{
   bool retVal = false;
   if( INITIAL_FD == mySocketFd )
   {
      mySocketFd   = clientFd;
      myRemoteHost = host;
      myRemotePort = port;
      retVal = true;
   }
   return retVal;
}

bool SocketBase::resetSocket()
{
   // Close the socket.  This is the only place where errors can occur.
   bool retVal = closeSocket();

   // Reset to INITIAL_FD to indicate that someone wants to reuse this instance.
   mySocketFd = INITIAL_FD;
   myRemoteHost.clear();
   myRemotePort = DEFAULT_PORT;

   return retVal;
}


//FIX we mean to have a recv() select() loop to collect N bytes as if MSG_WAITALL,
//    but without that so we can be sensitive to external signal(s) that may
//    abort the effort.  applicable only to TCP and to sockets in non-blocking mode.
//    return all-or-none.

int32_t SocketBase::recvsel( void *p_data, int32_t needLength, bool &quiter )
{
   int32_t             byteCount = 0;
   char                *p_buffer = ( char * )p_data;
   socklen_t             nameLen = sizeof( struct sockaddr );
   struct sockaddr *p_remoteInfo = NULL;
   int32_t            haveLength = 0;


   // 0  typically means the connection is closed.
   // Hopefully, this while loop will only execute once 99% of the time.
   if( NULL == p_data )
   {
      return 0;  // "ERROR - no receive buffer provided"
   }
   if( 0 >= mySocketFd )
   {
      return 0;  // "ERROR - socket is closed"
   }

   while( haveLength < needLength )
   {
      // Attempt to read the entire message.
      byteCount = recvfrom( mySocketFd,
                            p_buffer,
                            needLength - haveLength,
                            0,
                            p_remoteInfo,
                            ( NULL == p_remoteInfo ) ? NULL : &nameLen );

      if( 0 == byteCount )
      {
         /* socket has closed */
         return 0;
      }

      if( quiter )
      {
         /* external interrupt or abort signal */
         return 0;
      }

      haveLength += byteCount;
      p_buffer += byteCount;
   }

   return haveLength;
}

