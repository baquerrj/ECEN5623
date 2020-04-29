
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
#include <sys/utsname.h>
#include <syslog.h>
#include <thread.h>
#include <time.h>
#include <unistd.h>

bool abortS1;
bool abortS2;
bool abortS3;

sem_t* semS1;
sem_t* semS2;
sem_t* semS3;

const char* host;
utsname hostName;
RingBuffer< V4l2::buffer_s > frameBuffer( 20 );
uint32_t FRAMES_TO_EXECUTE = DEFAULT_FRAMES;
logging::config_s config   = {logging::LogLevel::INFO, "server.log"};

pthread_mutex_t ringLock;

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
   config.file          = fileName;
   if ( cmdOptionExists( argv, argv + argc, "-v" ) )
   {
      config.cutoff = logging::LogLevel::TRACE;
   }

   printf( "config.cutoff = %u\n", (int)config.cutoff );
   printf( "config.file = %s\n", config.file.c_str() );
   uname( &hostName );
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

   double serviceDeadline = 1 / (double)captureFrequency;
   double sequencerDeadline = 1 /(double)Sequencer::SEQUENCER_FREQUENCY;

   pthread_mutex_init( &ringLock, NULL );
   FrameCollector* fc          = new FrameCollector( 0 );
   FrameProcessor* fp          = new FrameProcessor();
   FrameSender* fs             = new FrameSender();
   Sequencer* sequencer        = new Sequencer( captureFrequency );
   pthread_t sequencerThreadId = sequencer->getThreadId();
   pthread_t processorThreadId = fp->getThreadId();
   pthread_t collectorThreadId = fc->getThreadId();
   pthread_t senderThreadId    = fs->getThreadId();

   fc->setDeadline( serviceDeadline );
   fp->setDeadline( serviceDeadline );
   fs->setDeadline( serviceDeadline );
   sequencer->setDeadline( sequencerDeadline );

   pthread_join( sequencerThreadId, NULL );
   sequencer->jitterAnalysis();
   delete sequencer;

   while ( !frameBuffer.isEmpty() )
   {
      sem_post( semS2 );
   }

   sem_post( semS1 );
   abortS1 = true;
   sem_post( semS2 );
   abortS2 = true;
   sem_post( semS3 );
   abortS3 = true;
   pthread_join( senderThreadId, NULL );
   pthread_join( processorThreadId, NULL );
   pthread_join( collectorThreadId, NULL );

   fc->terminate();
   delete fc;
   delete fp;
   delete fs;
   sem_close( semS1 );
   sem_close( semS2 );
   sem_close( semS3 );

   sem_unlink( SEMS1_NAME );
   sem_unlink( SEMS2_NAME );
   sem_unlink( SEMS3_NAME );

   pthread_mutex_destroy( &ringLock );

   return 0;
}