
#include <logging.h>
#include <signal.h>
#include <sockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread.h>
#include <time.h>
#include <unistd.h>
#include <syslog.h>
#include <Sequencer.h>
#include <FrameProcessor.h>
#include <FrameCollector.h>
#include <V4l2.h>
int force_format = 1;


sem_t* semS1;
sem_t* semS2;

const char *host;

static void doSocket( void )
{
   int client = -1;

   sockets::SocketServer *server = new sockets::SocketServer( std::string( host ), sockets::DEFAULTPORT );

   server->listen( 1 );

   while ( 0 > client )
   {
      client = server->accept();
   }

   const char *patterns[] = {
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

int main( int argc, char *argv[] )
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


   semS1 = sem_open( SEMS1_NAME, O_CREAT | O_EXCL, 0644, 0 );
   if ( semS1 == SEM_FAILED )
   {
      perror( "Failed to initialize S1 semaphore\n" );
      exit( -1 );
   }

   semS2 = sem_open( SEMS2_NAME, O_CREAT | O_EXCL, 0644, 0 );
   if ( semS2 == SEM_FAILED  )
   {
      perror( "Failed to initialize S2 semaphore\n" );
      exit( -1 );
   }

   FrameCollector* fc = new FrameCollector( 0 );
   FrameProcessor* fp = new FrameProcessor(  );
   Sequencer* sequencer = new Sequencer( 1 );
   pthread_t sequencerThreadId = sequencer->getThreadId();

   pthread_join( sequencerThreadId, NULL );

   sem_close( semS1 );
   sem_close( semS2 );

   sem_unlink( SEMS1_NAME );
   sem_unlink( SEMS2_NAME );
   return 0;
}