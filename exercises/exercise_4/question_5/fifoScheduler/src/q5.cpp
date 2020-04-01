#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>

#include "canny.h"
#include "common.h"
#include "hough.h"
#include "houghElliptical.h"
#include "logging.h"

int width  = HRES;
int height = VRES;

std::string WindowSize;

int lowThreshold     = 0;
int max_lowThreshold = 100;

bool isTimeToDie;
threadConfig_s* threadConfigs;
TransformAnalysis_s* threadAnalysis;

pthread_mutex_t captureLock;
pthread_mutex_t windowLock;
CvCapture* capture;

sem_t syncThreads[ THREAD_MAX ];

static const char* defaultDevice = "/dev/video0";

static void signalHandler( int signo )
{
   switch ( signo )
   {
      case SIGINT:
      {
         logging::INFO( "SIGINT Caught! Exiting...", true );
         isTimeToDie = true;
         break;
      }
      case SIGTERM:
      {
         logging::INFO( "SIGTERM Caught! Exiting...", true );
         isTimeToDie = true;
         break;
      }
      default:
      {
         logging::INFO( "Unknown signal caught!" );
         isTimeToDie = true;
         break;
      }
   }
}

void* ( *thread_entry_fn[ THREAD_MAX ] )( void* ) = {
    executeCanny,
    executeHough,
    executeHoughElliptical};

char* getCmdOption( char** begin, char** end, const std::string& option )
{
   char** itr = std::find( begin, end, option );
   if ( itr != end && ++itr != end )
   {
      return *itr;
   }
   return 0;
}

bool cmdOptionExists( char** begin, char** end, const std::string& option )
{
   return std::find( begin, end, option ) != end;
}

void printHelp( int argc, char* argv[] )
{
   if ( cmdOptionExists( argv, argv + argc, "-h" ) or cmdOptionExists( argv, argv + argc, "--help" ) )
   {
      printf( "Usage:\n" );
      printf( "-h, --help\t\tDisplay this help text and exit.\n" );
      printf( "-d, --device DEVICE\tPath to the device to use.\n" );
      printf( "-g, --geometry WxH\tWidth and Height in pixels for capture.\n" );
      printf( "-C, --canny\t\tPerform Canny Interactive Transformation.\n" );
      printf( "-L, --hough-line\tPerform Hough Straight Line Interactive Transformation.\n" );
      printf( "-E, --hough-elliptical\tPerform Hough Elliptical Interactive Transformation.\n" );
      exit( 0 );
   }
}

void createThreads( int device )
{
#ifdef USE_FIFO
   int max_prio = sched_get_priority_max( SCHED_FIFO );
   /* Initialize threads and attributes */
   if ( threadConfigs[ THREAD_CANNY ].isActive )
   {
      pthread_attr_init( &threadConfigs[ THREAD_CANNY ].atrributes );
      pthread_attr_setinheritsched( &threadConfigs[ THREAD_CANNY ].atrributes, PTHREAD_EXPLICIT_SCHED );
      pthread_attr_setschedpolicy( &threadConfigs[ THREAD_CANNY ].atrributes, SCHED_FIFO );
      threadConfigs[ THREAD_CANNY ].params.sched_priority = max_prio - 1;
      pthread_attr_setschedparam( &threadConfigs[ THREAD_CANNY ].atrributes, &threadConfigs[ THREAD_CANNY ].params );
   }

   if ( threadConfigs[ THREAD_HOUGHL ].isActive )
   {
      pthread_attr_init( &threadConfigs[ THREAD_HOUGHL ].atrributes );
      pthread_attr_setinheritsched( &threadConfigs[ THREAD_HOUGHL ].atrributes, PTHREAD_EXPLICIT_SCHED );
      pthread_attr_setschedpolicy( &threadConfigs[ THREAD_HOUGHL ].atrributes, SCHED_FIFO );
      threadConfigs[ THREAD_HOUGHL ].params.sched_priority = max_prio - 3;
      pthread_attr_setschedparam( &threadConfigs[ THREAD_HOUGHL ].atrributes, &threadConfigs[ THREAD_HOUGHL ].params );
   }

   if ( threadConfigs[ THREAD_HOUGHE ].isActive )
   {
      pthread_attr_init( &threadConfigs[ THREAD_HOUGHE ].atrributes );
      pthread_attr_setinheritsched( &threadConfigs[ THREAD_HOUGHE ].atrributes, PTHREAD_EXPLICIT_SCHED );
      pthread_attr_setschedpolicy( &threadConfigs[ THREAD_HOUGHE ].atrributes, SCHED_FIFO );
      threadConfigs[ THREAD_HOUGHE ].params.sched_priority = max_prio - 2;
      pthread_attr_setschedparam( &threadConfigs[ THREAD_HOUGHE ].atrributes, &threadConfigs[ THREAD_HOUGHE ].params );
   }
#endif

   for ( int i = 0; i < THREAD_MAX; ++i )
   {
      if ( threadConfigs[ i ].isActive == true )
      {
         if ( pthread_create( &threadConfigs[ i ].thread,
                              &threadConfigs[ i ].atrributes,
                              thread_entry_fn[ i ],
                              NULL ) != 0 )
         {
            perror( "ERROR: pthread_create:" );
            exit( -1 );
         }
         else
         {
            threadConfigs[ i ].isAlive = true;
         }
      }
   }
}

void initializeSemaphores()
{
   for ( int task = 0; task < THREAD_MAX; task++ )
   {
      if ( threadConfigs[ task ].isActive )
      {
         sem_init( &syncThreads[ task ], 0, 0 );
      }
   }
}

void destroySemaphores()
{
   for ( int task = 0; task < THREAD_MAX; task++ )
   {
      if ( threadConfigs[ task ].isActive )
      {
         sem_destroy( &syncThreads[ task ] );
      }
   }
}

double delta_t( struct timespec* stop, struct timespec* start )
{
   double current = ( (double)stop->tv_sec * 1000.0 ) +
                    ( (double)( (double)stop->tv_nsec / 1000000.0 ) );
   double last = ( (double)start->tv_sec * 1000.0 ) +
                 ( (double)( (double)start->tv_nsec / 1000000.0 ) );
   return ( current - last );
}

int main( int argc, char* argv[] )
{
   char jitter[ 100 ];
   char missed_deadline[ 100 ];
   // See if we need to print help first. No use in
   // allocating memory and registering signals
   // if we're just going to exit right away
   printHelp( argc, argv );

   // Preliminary stuff
   pid_t mainThreadId = getpid();
   signal( SIGINT, signalHandler );
   signal( SIGTERM, signalHandler );

   threadConfigs  = new threadConfig_s[ THREAD_MAX ];
   threadAnalysis = new TransformAnalysis_s[ THREAD_MAX ];
   memset( threadAnalysis, 0, ( THREAD_MAX ) * sizeof( TransformAnalysis_s ) );

   // Configure logger with logging level
   std::string fileName     = "capture" + std::to_string( mainThreadId ) + ".log";
   logging::config_s config = {logging::LogLevel::TRACE, fileName};
   logging::configure( config );

   char* deviceName = getCmdOption( argv, argv + argc, "-d" );
   if ( !deviceName )
   {
      deviceName = getCmdOption( argv, argc + argv, "--device" );
   }

   threadConfigs[ THREAD_CANNY ].isActive = ( cmdOptionExists( argv, argv + argc, "-C" ) or
                                              cmdOptionExists( argv, argv + argc, "--canny" ) );

   threadConfigs[ THREAD_HOUGHL ].isActive = ( cmdOptionExists( argv, argv + argc, "-L" ) or
                                               cmdOptionExists( argv, argv + argc, "--hough-line" ) );

   threadConfigs[ THREAD_HOUGHE ].isActive = ( cmdOptionExists( argv, argv + argc, "-E" ) or
                                               cmdOptionExists( argv, argv + argc, "--hough-elliptical" ) );

   char* geometry = getCmdOption( argv, argv + argc, "-g" );
   if ( !geometry )
   {
      geometry = getCmdOption( argv, argv + argc, "--geometry" );
   }

   if ( geometry )
   {
      sscanf( geometry, "%dx%d", &width, &height );
   }

   WindowSize = std::to_string( width ) + "x" + std::to_string( height );
   int device = -1;
   struct stat buffer;

   if ( deviceName )
   {
      if ( 0 == stat( deviceName, &buffer ) )
      {
         sscanf( deviceName, "%d", &device );
         logging::log( "Using:\n\tdevice = " + std::string( deviceName ) + "\n",
                       true );
      }
   }
   else if ( 0 == stat( defaultDevice, &buffer ) )
   {
      sscanf( defaultDevice, "%d", &device );
      logging::log( "Using:\n\tdevice = " + std::string( defaultDevice ) + "\n",
                    true );
   }
   else
   {
      logging::ERROR( "Device does not exist!", true );
      return 1;
   }

   logging::log( "\tgeometry = " + std::to_string( width ) + "x" + std::to_string( height ) + "\n",
                 true );

   //initializeSemaphores();

   capture = (CvCapture*)cvCreateCameraCapture( device );
   cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, width );
   cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, height );

   pthread_mutex_init( &captureLock, NULL );
   pthread_mutex_init( &windowLock, NULL );

   logging::INFO( "Starting Threads!" );
   isTimeToDie = false;
   createThreads( device );

#ifdef SHOW_WINDOWS
   if ( threadConfigs[ THREAD_CANNY ].isActive )
   {
      cv::namedWindow( window_name[ THREAD_CANNY ], CV_WINDOW_AUTOSIZE );
      // Create a Trackbar for user to enter threshold
      cv::createTrackbar( "Min Threshold:", window_name[ THREAD_CANNY ], &lowThreshold, max_lowThreshold, CannyThreshold );
      logging::INFO( "Spawned " + std::string( window_name[ THREAD_CANNY ] ), true );
   }
   if ( threadConfigs[ THREAD_HOUGHL ].isActive )
   {
      cv::namedWindow( window_name[ THREAD_HOUGHL ], CV_WINDOW_AUTOSIZE );
      logging::INFO( "Spawned " + std::string( window_name[ THREAD_HOUGHL ] ), true );
   }
   if ( threadConfigs[ THREAD_HOUGHE ].isActive )
   {
      cv::namedWindow( window_name[ THREAD_HOUGHE ], CV_WINDOW_AUTOSIZE );
      logging::INFO( "Spawned " + std::string( window_name[ THREAD_HOUGHE ] ), true );
   }
#endif
   //semPost( THREAD_CANNY );

   // Join threads and destroy windows
   for ( int thread = 0; thread < THREAD_MAX; thread++ )
   {
      if ( threadConfigs[ thread ].isActive and threadConfigs[ thread ].isAlive )
      {
         pthread_join( threadConfigs[ thread ].thread, NULL );

#ifdef SHOW_WINDOWS
         cvDestroyWindow( window_name[ thread ] );
#endif
      }
   }

   for ( int thread = 0; thread < THREAD_MAX; thread++ )
   {
      if ( threadConfigs[ thread ].isActive )
      {
         snprintf( jitter, sizeof( jitter ), "%s Positive Jitter = %0.2lfms", thread_name[ thread ], ( threadAnalysis[ thread ].pos_jitter ) );
         logging::INFO( jitter, true );
         snprintf( jitter, sizeof( jitter ), "%s Negative Jitter = %0.2lfms ", thread_name[ thread ], ( threadAnalysis[ thread ].neg_jitter ) );
         logging::INFO( jitter, true );
         snprintf( missed_deadline, sizeof( missed_deadline ), "%s Total Missed Deadline = %d/%d frames", thread_name[ thread ], threadAnalysis[ thread ].deadline_missed, FRAMES_TO_EXECUTE );
         logging::INFO( missed_deadline, true );
      }
   }
   //destroySemaphores();
   logging::INFO( "Exiting!", true );

   delete threadConfigs;

   pthread_mutex_destroy( &captureLock );
   pthread_mutex_destroy( &windowLock );

   cvReleaseCapture( &capture );
   return 0;
}
