//#include <FrameCollector.h>
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
#include <FrameCollector.h>

int force_format = 1;

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

#if 0
   FrameCollector* fc = &getCollector( 0 );

   while ( fc->frameCount < FRAMES_TO_EXECUTE );

   fc->terminate();
   //delete fc;
#else
   V4l2* v4l2 = new V4l2( "/dev/video0" );

   unsigned int count = 0;
   struct timespec read_delay;
   struct timespec time_error;

   read_delay.tv_sec  = 0;
   read_delay.tv_nsec = 30000;

   while ( count < FRAMES_TO_EXECUTE )
   {
      if ( v4l2->readFrame() )
      {
         if ( nanosleep( &read_delay, &time_error ) != 0 )
            perror( "nanosleep" );
         else
            printf( "time_error.tv_sec=%ld, time_error.tv_nsec=%ld\n", time_error.tv_sec, time_error.tv_nsec );

         count++;
      }
   }

   v4l2->stopCapture();
   delete v4l2;
#endif
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
