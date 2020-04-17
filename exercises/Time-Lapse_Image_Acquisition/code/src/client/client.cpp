#include <logging.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sockets.h>

int main( void )
{
   pid_t mainThreadId       = getpid();
   std::string fileName     = "client" + std::to_string( mainThreadId ) + ".log";
   logging::config_s config = {logging::LogLevel::TRACE, fileName};
   logging::configure( config );

   logging::INFO( "CLIENT HERE!", true );

   SocketReceiver* receiver = new SocketReceiver();
   logging::INFO( std::string("SocketReceiver ") + receiver->getLocalAddress(), true );
   return 0;
}
