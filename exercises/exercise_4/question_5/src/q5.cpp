#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "canny.h"
#include "common.h"
#include "hough.h"
#include "logging.h"

int width  = HRES;
int height = VRES;

int lowThreshold     = 0;
int max_lowThreshold = 100;

bool isTimeToDie;
threadConfig_s* threadConfigs;
pid_t mainThreadId;

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
#if 0

   int max_prio  = sched_get_priority_max( SCHED_FIFO );
   int min_prio  = sched_get_priority_min( SCHED_FIFO );
   /*Initialise threads and attributes*/
   pthread_attr_init( &threadConfigs[ THREAD_MAIN ].atrributes );
   pthread_attr_setinheritsched( &threadConfigs[ THREAD_MAIN ].atrributes, PTHREAD_EXPLICIT_SCHED );
   pthread_attr_setschedpolicy( &threadConfigs[ THREAD_MAIN ].atrributes, SCHED_FIFO );
   threadConfigs[ THREAD_MAIN ].params.sched_priority = max_prio;

   if ( doCanny )
   {
      pthread_attr_init( &threadConfigs[ THREAD_CANNY ].atrributes );
      pthread_attr_setinheritsched( &threadConfigs[ THREAD_CANNY ].atrributes, PTHREAD_EXPLICIT_SCHED );
      pthread_attr_setschedpolicy( &threadConfigs[ THREAD_CANNY ].atrributes, SCHED_FIFO );
      threadConfigs[ THREAD_CANNY ].params.sched_priority = max_prio;
      pthread_attr_setschedparam( &threadConfigs[ THREAD_CANNY ].atrributes, &threadConfigs[ THREAD_CANNY ].params );
   }

   if ( doHoughLine )
   {
      pthread_attr_init( &threadConfigs[ THREAD_HOUGHL ].atrributes );
      pthread_attr_setinheritsched( &threadConfigs[ THREAD_HOUGHL ].atrributes, PTHREAD_EXPLICIT_SCHED );
      pthread_attr_setschedpolicy( &threadConfigs[ THREAD_HOUGHL ].atrributes, SCHED_FIFO );
      threadConfigs[ THREAD_HOUGHL ].params.sched_priority = max_prio;
   }

   if ( doHoughElliptical )
   {
      pthread_attr_init( &threadConfigs[ THREAD_HOUGHE ].atrributes );
      pthread_attr_setinheritsched( &threadConfigs[ THREAD_HOUGHE ].atrributes, PTHREAD_EXPLICIT_SCHED );
      pthread_attr_setschedpolicy( &threadConfigs[ THREAD_HOUGHE ].atrributes, SCHED_FIFO );
      threadConfigs[ THREAD_HOUGHE ].params.sched_priority = max_prio;
   }
#endif

   for ( int i = 0; i < THREAD_MAX; ++i )
   {
      if ( threadConfigs[ i ].isActive == true )
      {
         if ( pthread_create( &threadConfigs[ i ].thread,
                              &threadConfigs[ i ].atrributes,
                              thread_entry_fn[ i ],
                              (void*)&device ) != 0 )
         {
            perror( "ERROR; pthread_create:" );
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
   // See if we need to print help first. No use in
   // allocating memory and registering signals
   // if we're just going to exit right away
   printHelp( argc, argv );

   // Preliminary stuff
   mainThreadId = getpid();
   signal( SIGINT, signalHandler );
   signal( SIGTERM, signalHandler );

   threadConfigs = new threadConfig_s[ THREAD_MAX ];

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

   initializeSemaphores();

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

   semPost( THREAD_CANNY );

   //sleep(3);
   //isTimeToDie = true;
   for ( int thread = 0; thread < THREAD_MAX; thread++ )
   {
      if ( threadConfigs[ thread ].isActive and threadConfigs[ thread ].isAlive )
      {
         pthread_join( threadConfigs[ thread ].thread, NULL );
      }
   }

   destroySemaphores();
   logging::INFO( "Exiting!", true );

   delete threadConfigs;

   cvReleaseCapture( &capture );
   return 0;
}
