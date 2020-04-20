#include <logging.h>
#include <signal.h>
#include <sockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <thread.h>
#include <unistd.h>

int main( int argc, char* argv[] )
{
   const char* host = getCmdOption( argv, argv + argc, "--host" );
   if ( !host )
   {
      host = sockets::LOCALHOST;
   }
   logging::INFO( "CLIENT CONNECT TO " + std::string( host ), true );
   pid_t mainThreadId       = getpid();
   std::string fileName     = "client" + std::to_string( mainThreadId ) + ".log";
   logging::config_s config = {logging::LogLevel::TRACE, fileName};
   logging::configure( config );

   //SocketClient* receiver = new SocketClient( "192.168.137.41", DEFAULTPORT );
   sockets::SocketClient* receiver = new sockets::SocketClient( host, sockets::DEFAULTPORT );

   receiver->connect();

   int recvs = 0;
   while ( recvs < 5 )
   {
      receiver->read();
      receiver->echo();
      recvs++;
   }
   delete receiver;
   return 0;
}
