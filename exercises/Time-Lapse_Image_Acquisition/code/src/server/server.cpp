
#include <FrameCollector.h>
#include <FrameProcessor.h>
#include <FrameSender.h>
#include <RingBuffer.h>
#include <Sequencer.h>
#include <SocketBase.h>
#include <SocketServer.h>
#include <V4l2.h>
#include <cleanup.h>
#include <fcntl.h>
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
bool abortS4;

sem_t* semS1;
sem_t* semS2;
sem_t* semS3;
sem_t* semS4;

const char* host;
utsname hostName;
RingBuffer< V4l2::buffer_s > frameBuffer( 25 );
uint32_t FRAMES_TO_EXECUTE = DEFAULT_FRAMES;
logging::config_s config   = {logging::LogLevel::ERROR, "server.log"};

pthread_mutex_t ringLock;

static void createSemaphoresAndMutexes()
{
   pthread_mutex_init( &ringLock, NULL );

   semS1 = sem_open( SEMS1_NAME, O_CREAT | O_EXCL, 0644, 0 );
   if ( semS1 == SEM_FAILED )
   {
      sem_unlink( SEMS1_NAME );
      semS1 = sem_open( SEMS1_NAME, O_CREAT | O_EXCL, 0644, 0 );
      if ( semS1 == SEM_FAILED )
      {
         perror( "Failed to initialize S1 semaphore" );
         exit( -1 );
      }
   }

   semS2 = sem_open( SEMS2_NAME, O_CREAT | O_EXCL, 0644, 0 );
   if ( semS2 == SEM_FAILED )
   {
      sem_unlink( SEMS2_NAME );
      semS2 = sem_open( SEMS2_NAME, O_CREAT | O_EXCL, 0644, 0 );
      if ( semS2 == SEM_FAILED )
      {
         perror( "Failed to initialize S2 semaphore" );
         exit( -1 );
      }
   }

   semS3 = sem_open( SEMS3_NAME, O_CREAT | O_EXCL, 0644, 0 );
   if ( semS3 == SEM_FAILED )
   {
      sem_unlink( SEMS3_NAME );
      semS3 = sem_open( SEMS3_NAME, O_CREAT | O_EXCL, 0644, 0 );
      if ( semS3 == SEM_FAILED )
      {
         perror( "Failed to initialize S3 semaphore" );
         exit( -1 );
      }
   }
   semS4 = sem_open( SEMS4_NAME, O_CREAT | O_EXCL, 0644, 0 );
   if ( semS4 == SEM_FAILED )
   {
      sem_unlink( SEMS4_NAME );
      semS4 = sem_open( SEMS4_NAME, O_CREAT | O_EXCL, 0644, 0 );
      if ( semS4 == SEM_FAILED )
      {
         perror( "Failed to initialize S3 semaphore" );
         exit( -1 );
      }
   }
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

   uname( &hostName );
   logging::configure( config );
   logging::INFO( "SERVER ON " + std::string( host ), true );

   double serviceDeadline   = 1 / (double)captureFrequency;
   double sequencerDeadline = 1 / (double)Sequencer::SEQUENCER_FREQUENCY;

   createSemaphoresAndMutexes();

   FrameCollector* fc          = new FrameCollector( 0 );
   FrameProcessor* fp          = new FrameProcessor();
   FrameSender* fs             = new FrameSender();
   Sequencer* sequencer        = new Sequencer( captureFrequency );
   Cleanup* cleaner            = new Cleanup( fc, fs );
   pthread_t sequencerThreadId = sequencer->getThreadId();
   pthread_t processorThreadId = fp->getThreadId();
   pthread_t collectorThreadId = fc->getThreadId();
   pthread_t senderThreadId    = fs->getThreadId();
   pthread_t cleanerThreadId   = cleaner->getThreadId();

   fc->setDeadline( serviceDeadline );
   fp->setDeadline( serviceDeadline );
   fs->setDeadline( serviceDeadline );
   sequencer->setDeadline( sequencerDeadline );

   pthread_join( sequencerThreadId, NULL );
   sequencer->jitterAnalysis();
   delete sequencer;

   // sem_post( semS1 );
   // abortS1 = true;
   // sem_post( semS2 );
   // abortS2 = true;
   // sem_post( semS3 );
   // abortS3 = true;
   // sem_post( semS4 );
   // abortS4 = true;
   pthread_join( senderThreadId, NULL );
   pthread_join( processorThreadId, NULL );
   pthread_join( collectorThreadId, NULL );
   pthread_join( cleanerThreadId, NULL );

   fc->terminate();

   fc->jitterAnalysis();
   fp->jitterAnalysis();
   fs->jitterAnalysis();
   delete fc;
   delete fp;
   delete fs;
   delete cleaner;
   sem_close( semS1 );
   sem_close( semS2 );
   sem_close( semS3 );
   sem_close( semS4 );

   sem_unlink( SEMS1_NAME );
   sem_unlink( SEMS2_NAME );
   sem_unlink( SEMS3_NAME );
   sem_unlink( SEMS4_NAME );

   pthread_mutex_destroy( &ringLock );

   return 0;
}