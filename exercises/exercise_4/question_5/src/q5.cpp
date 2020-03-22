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

int width  = HRES;
int height = VRES;

bool doCanny           = false;
bool doHoughLine       = false;
bool doHoughElliptical = false;

threadConfig_s* threadConfigs;

void* dummy( void* args )
{
   return NULL;
}

void* ( *thread_entry_fn[ THREAD_MAX ] )( void* ) = {
    executeCanny,
    executeHough,
    executeHoughElliptical,
    dummy};

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

int main( int argc, char* argv[] )
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
      return 0;
   }

   if ( not( cmdOptionExists( argv, argv + argc, "-d" ) or cmdOptionExists( argv, argv + argc, "--device" ) ) )
   {
      printf( "Must specify a device to use!\n" );
      return 0;
   }

   char* deviceName = getCmdOption( argv, argv + argc, "-d" );
   if ( !deviceName )
   {
      deviceName = getCmdOption( argv, argc + argv, "--device" );
   }

   doCanny = ( cmdOptionExists( argv, argv + argc, "-C" ) or
               cmdOptionExists( argv, argv + argc, "--canny" ) );

   doHoughLine = ( cmdOptionExists( argv, argv + argc, "-L" ) or
                   cmdOptionExists( argv, argv + argc, "--hough-line" ) );

   doHoughElliptical = ( cmdOptionExists( argv, argv + argc, "-E" ) or
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
   if ( deviceName )
   {
      struct stat buffer;
      if ( 0 == stat( deviceName, &buffer ) )
      {
         printf( "exists!\n" );
         sscanf( deviceName, "%d", &device );
      }
      else
      {
         printf( "does not exist!\n" );
      }
   }

   threadConfigs                           = new threadConfig_s[ THREAD_MAX ];
   threadConfigs[ THREAD_CANNY ].isActive  = doCanny;
   threadConfigs[ THREAD_HOUGHL ].isActive = doHoughLine;
   threadConfigs[ THREAD_HOUGHE ].isActive = doHoughElliptical;
   createThreads( device );

   printf( "Using:\n\tdevice = %s\n", deviceName );
   printf( "\tgeometry = %dx%d\n", width, height );

   pthread_join( threadConfigs[ THREAD_CANNY ].thread, NULL );
   pthread_join( threadConfigs[ THREAD_HOUGHE ].thread, NULL );
   pthread_join( threadConfigs[ THREAD_HOUGHL ].thread, NULL );

   delete threadConfigs;
#if 0
   CvCapture* capture = NULL;
   if ( doCanny )
   {
      capture = (CvCapture*)executeCanny( device );
   }
   else if ( doHoughLine )
   {
      capture = (CvCapture*)executeHough( device );
   }
   else if ( doHoughElliptical )
   {
      capture = (CvCapture*)executeHoughElliptical( device );
   }
   cvReleaseCapture( &capture );
#endif
   return 0;
}
