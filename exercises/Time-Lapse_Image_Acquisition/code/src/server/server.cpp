
#include <FrameCollector.h>
#include <FrameProcessor.h>
#include <FrameSender.h>
#include <RingBuffer.h>
#include <Sequencer.h>
#include <SocketBase.h>
#include <SocketServer.h>
#include <V4l2.h>
#include <logging.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <thread.h>
#include <time.h>
#include <unistd.h>

int force_format = 1;

sem_t* semS1;
sem_t* semS2;
sem_t* semS3;

const char* host;

RingBuffer< V4l2::buffer_s > frameBuffer( 20 );
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
   uint8_t captureFrequency = 1;
   if ( cmdOptionExists( argv, argv + argc, "-f" ) )
   {
      captureFrequency = std::atoi( getCmdOption( argv, argv + argc, "-f" ) );
   }

   if ( cmdOptionExists( argv, argv + argc, "-n" ) )
   {
      FRAMES_TO_EXECUTE = std::atoi( getCmdOption( argv, argv + argc, "-n" ) );
   }

   printf( "FRAMES_TO_EXECUTE = %u\n", FRAMES_TO_EXECUTE );

   pid_t mainThreadId   = getpid();
   std::string fileName = "server" + std::to_string( mainThreadId ) + ".log";

   logging::config_s config = {logging::LogLevel::INFO, fileName};
   if ( cmdOptionExists( argv, argv + argc, "-v" ) )
   {
      logging::config_s config = {logging::LogLevel::TRACE, fileName};
   }

   logging::configure( config );
   logging::INFO( "SERVER ON " + std::string( host ), true );

   semS1 = sem_open( SEMS1_NAME, O_CREAT, 0644, 0 );
   if ( semS1 == SEM_FAILED )
   {
      perror( "Failed to initialize S1 semaphore\n" );
      exit( -1 );
   }

   semS2 = sem_open( SEMS2_NAME, O_CREAT, 0644, 0 );
   if ( semS2 == SEM_FAILED )
   {
      perror( "Failed to initialize S2 semaphore\n" );
      exit( -1 );
   }

   semS3 = sem_open( SEMS3_NAME, O_CREAT, 0644, 0 );
   if ( semS3 == SEM_FAILED )
   {
      perror( "Failed to initialize S3 semaphore\n" );
      exit( -1 );
   }

   FrameCollector* fc          = new FrameCollector( 0 );
   FrameProcessor* fp          = new FrameProcessor();
   FrameSender* fs             = new FrameSender();
   Sequencer* sequencer        = new Sequencer( captureFrequency );
   pthread_t sequencerThreadId = sequencer->getThreadId();

   pthread_join( sequencerThreadId, NULL );
   while ( !frameBuffer.isEmpty() )
   {
      sem_post( semS2 );
   }
   //fc->terminate();
   //delete fc;
   delete fp;
   delete sequencer;

   sem_close( semS1 );
   sem_close( semS2 );
   sem_close( semS3 );

   sem_unlink( SEMS1_NAME );
   sem_unlink( SEMS2_NAME );
   sem_unlink( SEMS3_NAME );
   return 0;
}