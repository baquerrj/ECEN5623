#ifndef __SOCKET_CLIENT_H__
#define __SOCKET_CLIENT_H__

#include <string>
#include <SocketBase.h>

class SocketClient: public SocketBase
{
   // Data Types
   public:
   protected:
   private:

   // Functions
   public:
      SocketClient( const SocketBase::socketType_e socketType = SocketBase::DEFAULT_SOCKET );
      virtual ~SocketClient();

      //------------------------------------------------------------------------
      //! Setup the client's socket.  This function will behave very differently
      //! depending on the type of socket (specified in the constructor), and
      //! what arguments are specified.
      //!
      //! If TCP - only the remote host, port, and nonBlocking arguments are used.
      //! If UDP - If local host and port are specified, the socket is bound
      //!          to that information for receiving.
      //!        - If remote host and port are specified, the socket is
      //!          connected (fixed) to that information for sending.
      //!
      //! Use a port value of 0 to indicate that any port the system chooses
      //! is acceptable.
      //!
      //! @note This method is made virtual for mock-up in unit test environemnt
      //!
      //! @note -
      //!  For UDP, if the local port is not specified, the caller doesn't want
      //!  their UDP socket to be bound.  This means the user either has no
      //!  intention of receiving information, or will send prior to receiving
      //!  (the act of sending causes the system to implicitly bind the socket).
      //!
      //! @param[in] nonBlocking - If true, all receive() calls will return
      //!               immediately if no data present.  Has no effect on
      //!               send().  Defaults to false.
      //! @param[in] remoteHost - If specified, fixes the remote host to communicate
      //!               with for this socket. If specified, port must also be
      //!               specified.
      //! @param[in] remotePort - If specified, fixes the remote port to communicate
      //!               with for this socket. If specified, host must also be
      //!               specified.  A value of -1 means to ignore the port, 0
      //!               means to let the system choose an available port.
      //! @param[in] localHost - UDP only - If specified, binds the socket to
      //!               localHost host and localPort port.  localPort must
      //!               also be specified for this argument to be used.  If
      //!               locaHost is empty, but localPort specified, 'localhost'
      //!               will be used.
      //! @param[in] localPort - UDP only - If specified, binds the socket to
      //!               localHost host and localPort port.  If locaHost is
      //!               empty, but localPort specified, 'localhost' will be
      //!               used.  A value of -1 means to ignore the port, 0 means
      //!               to let the system choose an available port.
      //------------------------------------------------------------------------
      virtual bool setupSocket( const std::string remoteHost = DEFAULT_HOST,
                                const int32_t remotePort = DEFAULT_PORT,
                                const bool nonBlocking = false,
                                const bool bindUdp = false );

      //------------------------------------------------------------------------
      // Only call this if a UDP socket wants to explicitly connect to a remote host/port.
      //------------------------------------------------------------------------
      bool lockUdpToRemoteSide( const std::string remoteHost, const int32_t remotePort );

   protected:
      //------------------------------------------------------------------------
      //! Connects the socket (see setupSocket()) to the address specified
      //! during setupSocket().  Used for both UDP and TCP socket types.  Not used
      //! for UDP broadcast.
      //!
      //! @returns 0 on success, non-0 on failure and sets myErrno.
      //------------------------------------------------------------------------
      bool connectSocket();

   private:
      SocketClient( const SocketClient &rhs );
      SocketClient &operator=( const SocketClient &rhs );

// Variables
   public:
   protected:
   private:
};

#endif // __SOCKET_CLIENT_H__
