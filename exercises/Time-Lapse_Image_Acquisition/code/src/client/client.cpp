#include <SocketBase.h>
#include <SocketClient.h>
#include <logging.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <thread.h>
#include <unistd.h>

int force_format = 1;

#define IMAGE_SIZE ( 921800 )
#define USEC_PER_MSEC ( 1000 )

const char* host;
std::string ppmName( "test_xxxxxxxx.ppm" );

bool abortSend = false;

uint32_t FRAMES_TO_EXECUTE = DEFAULT_FRAMES;

static void doSocket( void )
{
   SocketClient* receiver = new SocketClient( SocketBase::TCP_SOCKET );
   receiver->setupSocket( std::string( host ), DEFAULT_PORT );

   char buffer[ IMAGE_SIZE ];
   uint32_t tag = 0;
   while ( tag < FRAMES_TO_EXECUTE )
   {
      int valread = 0;
      sprintf( &ppmName.front(), "test_%08d.ppm", tag );
      std::ofstream file;
      file.open( ppmName, std::ofstream::out );
      logging::INFO( "Receiving image...", true );

      valread = receiver->recvsel( (void*)buffer, sizeof( buffer ), abortSend );

      logging::INFO( "Image " + std::to_string( tag ) + " Bytes Read " + std::to_string( valread ), true );

      file.write( buffer, sizeof( buffer ) );

      memset( &buffer[ 0 ], 0, sizeof( buffer ) );
      file.close();
      tag++;
   }
   delete receiver;
}

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

   doSocket();
   return 0;
}
