#include <FrameCollector.h>
#include <RingBuffer.h>
#include <V4l2.h>
#include <common.h>
#include <logging.h>
#include <syslog.h>
#include <thread.h>

extern pthread_mutex_t ringLock;
extern bool abortS1;
extern sem_t* semS1;
static const ProcessParams collectorParams = {
    cpuCollector,
    SCHED_FIFO,
    98,
    0};

static const ThreadConfigData collectorThreadConfig = {
    true,
    "COLLECTOR",
    collectorParams};

extern RingBuffer< V4l2::buffer_s > frameBuffer;

FrameCollector::FrameCollector( int device = 0 ) :
    FrameBase( collectorThreadConfig )
{
   name = collectorThreadConfig.threadName;
   if ( 0 > sem_init( &sem, 0, 0 ) )
   {
      perror( "FC sem_init failed" );
      exit( EXIT_FAILURE );
   }
   executionTimes = new double[ FRAMES_TO_EXECUTE * 20 ]{};
   if ( executionTimes == NULL )
   {
      logging::ERROR( "Mem allocation failed for executionTimes for FC" );
   }

   startTimes = new double[ FRAMES_TO_EXECUTE * 20 ]{};
   if ( startTimes == NULL )
   {
      logging::ERROR( "Mem allocation failed for startTimes for FC" );
   }

   endTimes = new double[ FRAMES_TO_EXECUTE * 20 ]{};
   if ( endTimes == NULL )
   {
      logging::ERROR( "Mem allocation failed for endTimes for FC" );
   }

   capture = new V4l2( "/dev/video" + std::to_string( device ), V4l2::IO_METHOD_USERPTR );
   if ( NULL == capture )
   {
      logging::ERROR( "Could not allocate memory for V4l2 Object", true );
      exit( EXIT_FAILURE );
   }
   thread = new CyclicThread( collectorThreadConfig, FrameCollector::execute, this, true );
   if ( NULL == thread )
   {
      logging::ERROR( "Could not allocate memory for FC Thread", true );
      exit( EXIT_FAILURE );
   }

   alive = true;
}

FrameCollector::~FrameCollector()
{
   logging::INFO( "FrameCollector::~FrameCollector() entered", true );
   if ( executionTimes )
   {
      delete executionTimes;
      executionTimes = NULL;
   }
   if ( startTimes )
   {
      delete startTimes;
      startTimes = NULL;
   }
   if ( endTimes )
   {
      delete endTimes;
      endTimes = NULL;
   }
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
   if ( abortS1 )
   {
      thread->shutdown();
      return;
   }
   sem_wait( semS1 );
   clock_gettime( CLOCK_REALTIME, &start );
   startTimes[ count ] = ( (double)start.tv_sec + (double)( ( start.tv_nsec ) / (double)1000000000 ) );  //Store start time in seconds

   // syslog( LOG_INFO, "S1 Count: %lld   %s Start Time: %lf seconds",
   //         count,
   //         name.c_str(),
   //         startTimes[ count ] );

   // struct timespec read_delay;
   // struct timespec time_error;

   // read_delay.tv_sec  = 0;
   // read_delay.tv_nsec = 30000;

   if ( frameCount < FRAMES_TO_EXECUTE )
   {
      V4l2::buffer_s* buffer = NULL;
      if ( NULL != ( buffer = capture->readFrame() ) )
      {
         //capture->processImage( buffer->start, buffer->length );
         if ( !frameBuffer.isFull() )
         {
            clock_gettime( CLOCK_REALTIME, &( buffer->timestamp ) );
            buffer->frameNumber = frameCount;
            logging::DEBUG( "S1 Count: " + std::to_string( frameCount ) + " added image to buffer" );
            pthread_mutex_lock( &ringLock );
            frameBuffer.enqueue( *buffer );
            pthread_mutex_unlock( &ringLock );
            frameCount++;
         }
         else
         {
            logging::WARN( name + " ring buffer FULL in cycle " + std::to_string( count ), true );
         }

         // if ( nanosleep( &read_delay, &time_error ) != 0 )
         // {
         //    perror( "nanosleep" );
         // }
      }
   }
   else
   {
      logging::INFO( "FC Collected " + std::to_string( frameCount ) + " frames", true );
      abortS1 = true;  // abort on next iteration
   }

   clock_gettime( CLOCK_REALTIME, &end );                                                          //Get end time of the service
   endTimes[ count ] = ( (double)end.tv_sec + (double)( ( end.tv_nsec ) / (double)1000000000 ) );  //Store end time in seconds

   executionTimes[ count ] = delta_t( &end, &start );

   syslog( LOG_INFO, "%s Count: %lld   C Time: %lf ms",
           name.c_str(),
           count,
           executionTimes[ count ] );

   logging::DEBUG( "S1 Count: " + std::to_string( count ) +
                   "   C Time: " + std::to_string( executionTimes[ count ] ) + " ms" );

   count++;  //Increment the count of service S1u
}
