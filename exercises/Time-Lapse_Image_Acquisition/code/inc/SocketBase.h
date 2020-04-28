#ifndef __SOCKET_BASE_H__
#define __SOCKET_BASE_H__

#include <netdb.h>
#include <stdint.h>
#include <sys/socket.h>

#include <string>

//! @brief Base Class for Sockets
class SocketBase
{
   //! @brief Public Data Types
public:
   //! Enum for types of sockets (can be a TCP or a UDP socket)
   enum socketType_e
   {
      TCP_SOCKET     = SOCK_STREAM,
      UDP_SOCKET     = SOCK_DGRAM,
      DEFAULT_SOCKET = TCP_SOCKET
   };

   //! Enum for port type for TCP sockets (server or client)
   enum portType_e
   {
      TCP_SERVER  = 0,
      TCP_CLIENT  = 1,
      TCP_INVALID = 2
   };

   //! @brief Public Functions
public:
   //! Constructor for SocketBase
   SocketBase( const SocketBase::socketType_e socketType = SocketBase::DEFAULT_SOCKET );

   //! Destructor - made virtual for children to override, if needed, and call parent's destructor
   virtual ~SocketBase();

   //! Returns the last errno value resulting from a failure.
   uint32_t getErrno();

   //------------------------------------------------------------------------
   //! Sets the internal errno value to -1 (to help differentiate
   //! between multiple errors over time).
   //------------------------------------------------------------------------
   void clearErrno();

   //! If the socket is still valid (assumed to be open), this function
   //! calls shutdown() to release all read and write operations, including
   //! accept() & recvfrom(), closes the socket, and sets the fd to CLOSING_FD.
   virtual bool closeSocket();

   //! @brief Receives data via socket
   //! Receive data from sender.  The data is stored in p_data, up to
   //! maxMsgLength.
   //!
   //! @param p_data          a buffer to store the incoming message.
   //! @param maxMsgLength    the max size of the buffer
   //! @param p_remoteHost    IP address of remote host
   //! @param p_remotePort    Port number of remote host
   //!
   //! @returns - The number of bytes received
   //------------------------------------------------------------------------
   virtual int32_t receive( void *p_data,
                            const int32_t &maxMsgLength,
                            std::string *p_remoteHost = NULL,
                            int32_t *p_remotePort     = NULL );

   //! @brief Receives data via socket
   //! Receive data from sender.  The data is stored in p_data, expected to
   //! be of size needLength.
   //!
   //! @param p_data          a buffer to store the incoming message.
   //! @param needLength      expected size of data to receive
   //! @param quitter         can be set by external caller(s) to abort a receive
   //!
   //! @returns - The number of bytes received.  virtually a blocking call
   //!            until the socket is closed by the peer, or an abort is
   //!            indicated, or the full measure of (needLength) bytes
   //!            have been received.
   int32_t recvsel( void *p_data, int32_t needLength, bool &quiter );

   //! @brief Sends data in buffer pointed to p_data
   //! Send data pointed to by p_data, for byteCount bytes, to
   //! recipient.
   //!
   //! @param p_data          a buffer containing the message to be transmitted.
   //! @param byteCount       The size of the message, in bytes, to transmit.
   //! @param p_remoteHost    IP address of recipient
   //! @param p_remotePort    Port number of recipient
   //!
   //! @returns - The number of bytes transmitted.
   virtual int32_t send( const void *p_data,
                         const int32_t &byteCount,
                         const std::string *p_remoteHost = NULL,
                         const int32_t *p_remotePort     = NULL );

   //! @brief Initialize data for client (port, host, and fd)
   virtual bool initializeClientData( const int32_t clientFd,
                                      const std::string &host,
                                      const int32_t &port );

   //! @brief Reset socket connection
   //!
   //! @returns true on sucessful reset
   virtual bool resetSocket();

   //! @returns std::string of the remote host's name, or "" if not
   //!   connected.
   const std::string &remoteHost();

   //! @returns port number of the port connected to on the remote host
   const int32_t &remotePort();

   //! @brief Check if socket is open
   //!
   //! @returns true if socket is still open
   virtual bool isAlive( void );

   //------------------------------------------------------------------------
   //! @sets the flags used in the send() function
   //------------------------------------------------------------------------
   void setSendFlags( int32_t flags )
   {
      sendFlags = flags;
   }

   //------------------------------------------------------------------------
   //! Set TCP_NODELAY socket option for TCP connections. When enabled,
   //! Nagle's algorithm is skipped, and each call to send() sends one packet
   //! of data ASAP.
   //!
   //! @note This method is made virtual for mock-up in unit test environemnt
   //!
   //! @warning This should only be set to true for interfaces that are
   //!          latency sensitive and never send a whole bunch of data all at
   //!          once. Rule of thumb is 2 kiB of data is considered "a bunch
   //!          of data". Otherwise, setting this to true may end up slowing
   //!          down communication.
   //!
   //! @param[in] enable : true to enable or false to disable
   //!
   //! @returns 0 on success or errno on failure
   //------------------------------------------------------------------------
   virtual int32_t setTcpNodelay( bool enable );

   //------------------------------------------------------------------------
   //! Sets receive timeout for blocking connections.
   //!
   //! @note This method is made virtual for mock-up in unit test environemnt
   //!
   //! @param[in] timeout : Timeout value in seconds
   //!
   //! @returns 0 on success or errno on failure
   //------------------------------------------------------------------------
   virtual int32_t setReceiveTimeout( double timeout );

protected:
   //------------------------------------------------------------------------
   //! Creates a socket if it doesn't already exist.
   //!
   //! @param[in] hostName - Name of the host to establish the socket with.
   //!                       If setting up a server-side socket, hostName
   //!                       should be "localhost".
   //! @param[in] port - Port to connect to.  For UDP, this is the port to
   //!                   send/receive with.  If set to 0, an available port will
   //!                   be automatically chosen.
   //!
   //! @returns true if a socket was properly created.  False otherwise.
   //!          Also returns false when a socket already exists.
   //------------------------------------------------------------------------
   bool setupSocket( const std::string hostName, const int32_t port = 0, const bool nonBlocking = false );

   //------------------------------------------------------------------------
   //! Assigns the address specified during setupSocket() the socket
   //! created during setupSocket()
   //!
   //! @returns 0 on success, non-0 on failure, and sets myErrno.
   //------------------------------------------------------------------------
   int32_t bindSocket();

   //------------------------------------------------------------------------
   //------------------------------------------------------------------------
   bool getAddressInformation( struct addrinfo &info, const std::string &host, const int32_t &port );

   //------------------------------------------------------------------------
   //------------------------------------------------------------------------
   bool getNameInformation( const struct sockaddr &info, std::string &host, int32_t &port );

private:
   //------------------------------------------------------------------------
   //------------------------------------------------------------------------
   SocketBase( const SocketBase &rhs );

   //------------------------------------------------------------------------
   //------------------------------------------------------------------------
   SocketBase &operator=( const SocketBase &rhs );

   ////////////////////////////////////////
   // Variables
   ////////////////////////////////////////
public:
   static const int32_t INITIAL_FD   = -1;
   static const int32_t CLOSING_FD   = -2;
   static const int32_t DEFAULT_PORT = -1;
#define DEFAULT_HOST ""

protected:
   //! Socket address information being used.  In the case of a server,
   //! this is the local host's information.  In the case of a client,
   //! this is the remote hosts's information.
   struct addrinfo myHostInfo;

   //! Local copy of errno.  Errno is a static variable that is available
   //! system-wide.  To help protect against different threads causing
   //! errno to update, this instance variable's used.
   int32_t myErrno;

   //! Socket file descriptor.  ID for the socket to use for communicating.
   int32_t mySocketFd;

   //! Remote host to which this socket is connected
   //! Only meaningful for TCP and connected UDP sockets.
   std::string myRemoteHost;

   //! Remote port to which this socket is connected
   //! Only meaningful for TCP and connected UDP sockets.
   int32_t myRemotePort;

   //! socket flags used in send( ) function
   int32_t sendFlags;

private:
};

inline void SocketBase::clearErrno()
{
   myErrno = -1;
}

inline uint32_t SocketBase::getErrno()
{
   return myErrno;
}

inline const std::string &SocketBase::remoteHost()
{
   return myRemoteHost;
}

inline const int32_t &SocketBase::remotePort()
{
   return myRemotePort;
}

inline bool SocketBase::isAlive()
{
   return ( 0 < mySocketFd );
}
#endif  // __SOCKET_BASE_H__
