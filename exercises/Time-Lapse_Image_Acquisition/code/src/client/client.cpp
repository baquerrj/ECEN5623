#include <logging.h>
#include <signal.h>
#include <sockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

extern const char* LOCALHOST;
extern const uint32_t DEFAULTPORT;

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

   while ( 1 )
   {
      receiver->read( 0 );
   }
   sleep( 30 );
   delete receiver;
   return 0;
}
