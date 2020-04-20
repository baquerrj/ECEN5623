#include <logging.h>
#include <signal.h>
#include <sockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int main( void )
{
   pid_t mainThreadId       = getpid();
   std::string fileName     = "client" + std::to_string( mainThreadId ) + ".log";
   logging::config_s config = {logging::LogLevel::TRACE, fileName};
   logging::configure( config );

   logging::INFO( "CLIENT HERE!", true );

   SocketClient* receiver = new SocketClient( "127.0.0.1", "8080" );
   logging::INFO( std::string( "SocketClient " ) + receiver->getLocalAddress(), true );

   receiver->connect();

   while ( 1 )
   {
      receiver->read( 0 );
   }
   sleep( 30 );
   delete receiver;
   return 0;
}
