#include <FrameCollector.h>
#include <logging.h>
#include <signal.h>
#include <sockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <thread.h>
#include <unistd.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

int main( int argc, char* argv[] )
{
   const char* host = getCmdOption( argv, argv + argc, "--host" );
   if ( !host )
   {
      host = sockets::LOCALHOST;
   }

   pid_t mainThreadId       = getpid();
   std::string fileName     = "server" + std::to_string( mainThreadId ) + ".log";
   logging::config_s config = {logging::LogLevel::TRACE, fileName};
   logging::configure( config );
   logging::INFO( "SERVER ON " + std::string( host ), true );

   printf( "SERVER HERE!\n" );

   FrameCollector* fc = &getCollector( 0 );

   while ( fc->frameCount < FRAMES_TO_EXECUTE );

   fc->terminate();
   delete fc;

   int client = -1;

   //SocketServer* server = new SocketServer( "192.168.137.41", DEFAULTPORT );
   sockets::SocketServer* server = new sockets::SocketServer( std::string( host ), sockets::DEFAULTPORT );

   logging::message_s* serverMessage = new logging::message_s;
   serverMessage->ThreadID           = THREAD_SERVER;
   serverMessage->level              = logging::LogLevel::TRACE;
   sprintf( serverMessage->msg, "test server" );

   logging::log( serverMessage );
   server->listen( 10 );

   while ( 0 > client )
   {
      logging::log( serverMessage );
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
      logging::log( serverMessage );

      server->send( client, patterns[ i ] );
      logging::log( serverMessage );

      server->read( client );
   }

   delete server;
   return 0;
}
