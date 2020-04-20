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


int main( void )
{
   pid_t mainThreadId       = getpid();
   std::string fileName     = "server" + std::to_string( mainThreadId ) + ".log";
   logging::config_s config = {logging::LogLevel::TRACE, fileName};
   logging::configure( config );

   printf( "SERVER HERE!\n" );

   int client = -1;

   SocketServer* server = new SocketServer( "192.168.137.41", DEFAULTPORT );

   server->listen( 10 );

   while ( 0 > client )
   {
      client = server->accept();
   }

   const char* patterns[] = {
       "Hello World",
       "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
       "12345671231243",
       "Another message!",
       "One more!"};

   for ( int i = 0; i < 5; i++ )
   {
      server->send( client, patterns[ i ] );
      server->read( client );
   }

   delete server;
   return 0;
}
