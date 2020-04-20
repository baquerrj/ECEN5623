#include <logging.h>
#include <signal.h>
#include <sockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <thread.h>
#include <unistd.h>

int main( void )
{
   pid_t mainThreadId       = getpid();
   std::string fileName     = "client" + std::to_string( mainThreadId ) + ".log";
   logging::config_s config = {logging::LogLevel::TRACE, fileName};
   logging::configure( config );

   logging::INFO( "CLIENT HERE!", true );

   SocketClient* receiver = new SocketClient( LOCALHOST, DEFAULTPORT );
   logging::INFO( std::string( "SocketClient " ) + receiver->getLocalAddress(), true );

   receiver->connect();

   int recvs = 0;
   while ( recvs < 5 )
   {
      receiver->read();
      receiver->echo();
      recvs++;
   }
   delete receiver;
   return 0;
}
