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

const char* host;
std::string ppmName( "test_xxxxxxxx.ppm" );

static void doSocket( void )
{
   SocketClient* receiver = new SocketClient( host, sockets::DEFAULTPORT );

   receiver->connect();

   char buffer[ IMAGE_SIZE ];
   int tag = 0;
   while ( tag < FRAMES_TO_EXECUTE )
   {
      int valread          = 0;
      int total_image_size = 0;
      sprintf( &ppmName.front(), "test_%08d.ppm", tag );
      FILE* fp = fopen( ppmName.c_str(), "w" );
      do
      {
         logging::INFO( "Receiving image...", true );
         valread = receiver->receive( (char*)buffer );
         logging::INFO( "Image " + std::to_string( tag ) + " Bytes Read " + std::to_string( valread ), true );
         total_image_size += valread;

         int write_size = fwrite( buffer, 1, valread, fp );
      } while (total_image_size < IMAGE_SIZE );

      fclose(fp);
      tag++;
   }
   delete receiver;
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
   std::string fileName     = "client" + std::to_string( mainThreadId ) + ".log";
   logging::config_s config = {logging::LogLevel::TRACE, fileName};
   logging::configure( config );
   logging::INFO( "CLIENT CONNECT TO " + std::string( host ), true );

   doSocket();
   return 0;
}
