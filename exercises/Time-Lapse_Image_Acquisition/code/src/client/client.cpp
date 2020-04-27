#include <logging.h>
#include <signal.h>
#include <SocketClient.h>
#include <SocketBase.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <thread.h>
#include <unistd.h>

int force_format = 1;

const char* host;

static void doSocket( void )
{
   SocketClient* receiver = new SocketClient( host, sockets::DEFAULTPORT );

   receiver->connect();

   int recvs = 0;
   while ( recvs < FRAMES_TO_EXECUTE )
   {
      receiver->read();
      receiver->echo();
      recvs++;
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
