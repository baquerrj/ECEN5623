#ifndef __SOCKET_SERVER_H__
#define __SOCKET_SERVER_H__

#include <string>
#include <SocketBase.h>

class SocketServer: public SocketBase
{
   // Data Types
   public:
   protected:
   private:

   // Functions
   public:
      SocketServer( const SocketBase::socketType_e socketType = SocketBase::DEFAULT_SOCKET );
      virtual ~SocketServer();

      bool setupSocket( const std::string localHost,
                        const int32_t localPort,
                        const uint32_t backlog,
                        const bool nonBlocking = false );

      //------------------------------------------------------------------------
      //! Extracts the first connection request on the queue of pending
      //! connections for the listening socket (see listenSocket), creates a
      //! new connected socket, and returns a new file descriptor referring to
      //! that socket.
      //!
      //! @returns -1 on error and sets myErrno.  All other values are valid
      //!          file descriptors
      //------------------------------------------------------------------------
      bool acceptSocket( SocketBase &client );


      //------------------------------------------------------------------------
      //! Overriding base class's version of this function to always return
      //! false.  No one should EVER call this function on a SocketServer.
      //------------------------------------------------------------------------
      bool initializeClientData( const int32_t clientFd, const std::string &host, const int32_t &port );

   protected:
   private:
      SocketServer( const SocketServer &rhs );
      SocketServer &operator=( const SocketServer &rhs );

      //------------------------------------------------------------------------
      //! Marks socket to be passive, this waiting for incoming connection
      //! requests.  Use acceptSocket() to accept the request.
      //!
      //! @returns 0 on success, non-0 on failure and sets myErrno.
      //------------------------------------------------------------------------
      int32_t listenSocket( int32_t backLog );

// Variables
   public:
   protected:
   private:
      int32_t myPort;
      uint32_t myBacklog;
};

#endif // __SOCKET_SERVER_H__