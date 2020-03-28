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
   for ( int tasks = 0; tasks < THREAD_MAX; tasks++ )
      sem_init( &syncThreads[ tasks ], 0, 0 );
}

int delta_t( struct timespec* stop, struct timespec* start, struct timespec* delta_t )
{
   int dt_sec  = stop->tv_sec - start->tv_sec;
   int dt_nsec = stop->tv_nsec - start->tv_nsec;

   if ( dt_sec >= 0 )
   {
      if ( dt_nsec >= 0 )
      {
         delta_t->tv_sec  = dt_sec;
         delta_t->tv_nsec = dt_nsec;
      }
      else
      {
         delta_t->tv_sec  = dt_sec - 1;
         delta_t->tv_nsec = NSEC_PER_SEC + dt_nsec;
      }
   }
   else
   {
      if ( dt_nsec >= 0 )
      {
         delta_t->tv_sec  = dt_sec;
         delta_t->tv_nsec = dt_nsec;
      }
      else
      {
         delta_t->tv_sec  = dt_sec - 1;
         delta_t->tv_nsec = NSEC_PER_SEC + dt_nsec;
      }
   }

   return ( 1 );
}

int main( int argc, char* argv[] )
{
   // Preliminary stuff
   mainThreadId = getpid();
   signal( SIGINT, signalHandler );
   signal( SIGTERM, signalHandler );

   threadConfigs = new threadConfig_s[ THREAD_MAX ];

   printHelp( argc, argv );

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
         printf( "Using:\n\tdevice = %s\n", deviceName );
      }
   }
   else if ( 0 == stat( defaultDevice, &buffer ) )
   {
      sscanf( defaultDevice, "%d", &device );
      printf( "Using:\n\tdevice = %s\n", defaultDevice );
   }
   else
   {
      logging::ERROR( "Device does not exist!", true );
      return 1;
   }

   printf( "\tgeometry = %dx%d\n", width, height );

   initializeSemaphores();

   capture = (CvCapture*)cvCreateCameraCapture( device );
   cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, width );
   cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, height );

   pthread_mutex_init( &captureLock, NULL );
   pthread_mutex_init( &windowLock, NULL );

   logging::INFO( "Starting Threads!" );
   isTimeToDie = false;
   createThreads( device );

   sem_post( &syncThreads[ THREAD_CANNY ] );

   //sleep(3);
   //isTimeToDie = true;
   pthread_join( threadConfigs[ THREAD_CANNY ].thread, NULL );
   pthread_join( threadConfigs[ THREAD_HOUGHE ].thread, NULL );
   pthread_join( threadConfigs[ THREAD_HOUGHL ].thread, NULL );

   logging::INFO( "Exiting!", true );

   delete threadConfigs;

   cvReleaseCapture( &capture );
   return 0;
}
