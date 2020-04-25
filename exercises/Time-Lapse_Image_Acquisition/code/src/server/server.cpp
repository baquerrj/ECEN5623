//#include <FrameCollector.h>
#include <FrameCollector.h>
#include <V4l2.h>
#include <logging.h>
#include <signal.h>
#include <sockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <thread.h>
#include <time.h>
#include <unistd.h>

int force_format = 1;

const char* host;

static void doSocket( void )
{
   int client = -1;

   sockets::SocketServer* server = new sockets::SocketServer( std::string( host ), sockets::DEFAULTPORT );

   server->listen( 1 );

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
}

int main( int argc, char* argv[] )
{
   bool local = cmdOptionExists( argv, argv + argc, "--local" );
   if ( local )
   {
      host = sockets::LOCALHOST;
   }
   else
   {
      host = "192.168.137.41";
   }

   pid_t mainThreadId       = getpid();
   std::string fileName     = "server" + std::to_string( mainThreadId ) + ".log";
   logging::config_s config = {logging::LogLevel::TRACE, fileName};
   logging::configure( config );
   logging::INFO( "SERVER ON " + std::string( host ), true );

   printf( "SERVER HERE!\n" );

   FrameCollector* fc = new FrameCollector( 0 );
   sleep(2);
   printf ("done collection\n");
   fc->terminate();
   printf( "terminated\n");
   delete fc;


   doSocket();

   printf( "returning...\n");
   return 0;
}
