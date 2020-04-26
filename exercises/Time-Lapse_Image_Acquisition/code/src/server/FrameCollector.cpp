#include <FrameCollector.h>
#include <V4l2.h>
#include <common.h>
#include <logging.h>
#include <thread.h>

extern sem_t* semS1;
static const ProcessParams collectorParams = {
    cpuMain,  // CPU1
    SCHED_FIFO,
    98,  // highest priority
    0};

static const ThreadConfigData collectorThreadConfig = {
    true,
    "COLLECTOR",
    collectorParams};

FrameCollector::FrameCollector( int device = 0 )
{
   if ( 0 > sem_init( &sem, 0, 0 ) )
   {
      perror( "FC sem_init failed" );
      exit( EXIT_FAILURE );
   }
   capture    = new V4l2( "/dev/video" + std::to_string( device ), V4l2::IO_METHOD_USERPTR );
   thread     = new CyclicThread( collectorThreadConfig, FrameCollector::execute, this, true );
   frameCount = 0;
}

FrameCollector::~FrameCollector()
{
   logging::INFO( "FrameCollector::~FrameCollector() entered", true );
   if ( thread )
   {
      delete thread;
      thread = NULL;
   }
   if ( capture )
   {
      delete capture;
      capture = NULL;
   }
   logging::INFO( "FrameCollector::~FrameCollector() exiting", true );
}

void FrameCollector::terminate()
{
   logging::INFO( "FrameCollector::terminate() entered", true );
   capture->stopCapture();
   logging::INFO( "FrameCollector::terminate() exiting", true );
}

void* FrameCollector::execute( void* context )
{
   ( (FrameCollector*)context )->collectFrame();
   return NULL;
}

void FrameCollector::collectFrame()
{
   sem_wait( semS1 );

   struct timespec read_delay;
   struct timespec time_error;

   read_delay.tv_sec  = 0;
   read_delay.tv_nsec = 30000;

   if ( frameCount < FRAMES_TO_EXECUTE )
   {
      V4l2::buffer_s* buffer = NULL;
      if ( NULL != ( buffer = capture->readFrame() ) )
      {
         capture->processImage( buffer->start, buffer->length );
         if ( nanosleep( &read_delay, &time_error ) != 0 )
         {
            perror( "nanosleep" );
         }
         else
         {
            printf( "time_error.tv_sec=%ld, time_error.tv_nsec=%ld\n", time_error.tv_sec, time_error.tv_nsec );
         }
         frameCount++;
      }
   }
}
