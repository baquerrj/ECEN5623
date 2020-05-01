#include <FrameReceiver.h>
#include <SocketBase.h>
#include <SocketClient.h>
#include <logging.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <thread.h>
#include <unistd.h>

#define IMAGE_SIZE ( 921800 )
#define USEC_PER_MSEC ( 1000 )

const char* host;

uint32_t FRAMES_TO_EXECUTE = DEFAULT_FRAMES;

int main( int argc, char* argv[] )
{
   bool local = cmdOptionExists( argv, argv + argc, "--local" );
   if ( local )
   {
      host = LOCAL_HOST.c_str();
   }
   else
   {
      host = "192.168.137.41";
   }
   if ( cmdOptionExists( argv, argv + argc, "-n" ) )
   {
      FRAMES_TO_EXECUTE = std::atoi( getCmdOption( argv, argv + argc, "-n" ) );
   }

   printf( "FRAMES_TO_EXECUTE = %u\n", FRAMES_TO_EXECUTE );
   pid_t mainThreadId       = getpid();
   std::string fileName     = "client" + std::to_string( mainThreadId ) + ".log";
   logging::config_s config = {logging::LogLevel::TRACE, fileName};
   logging::configure( config );
   logging::INFO( "CLIENT CONNECT TO " + std::string( host ), true );

   FrameReceiver* receiver = new FrameReceiver();

   sleep(1);

   receiver->setDeadline( 0 );

   pthread_join( receiver->getThreadId(), NULL );

   delete receiver;

   return 0;
}
