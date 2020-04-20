#include <logging.h>
#include <signal.h>
#include <sockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <thread.h>
#include <unistd.h>

static const ThreadConfigData threadConfigData = {
    true,
    "threadOne",
    DEFAULT_PROCESS_PARAMS};

static void* thread_fn( void* args )
{
   //logging::INFO( "THREADONE!" );
}

int main( void )
{
   pid_t mainThreadId       = getpid();
   std::string fileName     = "server" + std::to_string( mainThreadId ) + ".log";
   logging::config_s config = {logging::LogLevel::TRACE, fileName};
   logging::configure( config );

   //CyclicThread* thread = static_cast< CyclicThread* >( new CyclicThread( threadConfigData,
    //                                                                      thread_fn, NULL, true ) );

   printf( "SERVER HERE!\n" );

   int client = -1;
   while ( 1 )
   {
      SocketServer* server = new SocketServer( "127.0.0.1", "8080" );

      server->listen( 3 );

      client = server->accept();
      if ( -1 != client )
      {
         server->send( client );
      }
   }
   //delete thread;

   return 0;
}
