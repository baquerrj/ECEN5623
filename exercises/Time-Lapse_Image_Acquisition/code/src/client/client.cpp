#include <logging.h>
#include <signal.h>
#include <sockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <thread.h>
#include <unistd.h>

int force_format = 1;

int main( int argc, char* argv[] )
{
   const char* host = getCmdOption( argv, argv + argc, "--host" );
   if ( !host )
   {
      host = sockets::LOCALHOST;
   }
   pid_t mainThreadId       = getpid();
   std::string fileName     = "client" + std::to_string( mainThreadId ) + ".log";
   logging::config_s config = {logging::LogLevel::TRACE, fileName};
   logging::configure( config );
   logging::INFO( "CLIENT CONNECT TO " + std::string( host ), true );

   //SocketClient* receiver = new SocketClient( "192.168.137.41", DEFAULTPORT );
   sockets::SocketClient* receiver = new sockets::SocketClient( host, sockets::DEFAULTPORT );

   logging::message_s* clientMessage = new logging::message_s;
   clientMessage->ThreadID           = THREAD_CLIENT;
   clientMessage->level              = logging::LogLevel::TRACE;
   sprintf( clientMessage->msg, "test server" );
   logging::log( clientMessage );

   receiver->connect();

   int recvs = 0;
   while ( recvs < 5 )
   {
      logging::log( clientMessage );
      receiver->read();
      logging::log( clientMessage );
      receiver->echo();
      recvs++;
   }
   delete receiver;
   return 0;
}
