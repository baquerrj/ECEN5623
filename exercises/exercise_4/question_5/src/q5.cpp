#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "common.h"
#include "canny.h"

IplImage* frame;
int width = HRES;
int height = VRES;

bool doCanny = false;
bool doHoughLine = false;
bool doHoughElliptical = false;

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

int main( int argc, char* argv[] )
{
   if ( cmdOptionExists( argv, argv + argc, "-h" ) or cmdOptionExists( argv, argv + argc, "--help" ) )
   {
      printf( "Usage:\n" );
      printf( "-h, --help\t\tDisplay this help text and exit.\n" );
      printf( "-d, --device DEVICE\tPath to the device to use.\n" );
      printf( "-g, --geometry WxH\tWidth and Height in pixels for capture.\n" );
      printf( "Only one of the following three:\n" );
      printf( "-C, --canny\t\tPerform Canny Interactive Transformation.\n" );
      printf( "-L, --hough-line\tPerform Hough Straight Line Interactive Transformation.\n" );
      printf( "-E, --hough-elliptical\tPerform Hough Elliptical Interactive Transformation.\n" );
      return 0;
   }

   if ( not ( cmdOptionExists( argv, argv + argc, "-d" ) or cmdOptionExists( argv, argv + argc, "--device" ) ) )
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
   if ( false == doCanny )
   {
      doHoughLine = ( cmdOptionExists( argv, argv + argc, "-L" ) or
                      cmdOptionExists( argv, argv + argc, "--hough-line" ) );
   }
   if ( ( false == doCanny ) and ( false == doHoughLine ) )
   {
      doHoughElliptical = ( cmdOptionExists( argv, argv + argc, "-E" ) or
                            cmdOptionExists( argv, argv + argc, "--hough-elliptical" ) );
   }

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

   printf( "Using:\n\tdevice = %s\n", deviceName );
   printf( "\tgeometry = %dx%d\n", width, height );
   if ( doCanny )
   {
      printf( "\ttransform = canny\n" );
   }
   else if( doHoughLine )
   {
      printf( "\ttransform = hough-line\n" );
   }
   else if ( doHoughElliptical )
   {
      printf( "\ttransform = hough-elliptical\n" );
   }
   else
   {
      printf( "something went wrong - no transform chosen!\n" );
      exit( -1 );
   }

   CvCapture* capture = NULL;
   if ( doCanny )
   {
      capture = (CvCapture*) executeCanny( device );
   }

   cvReleaseCapture( &capture );
   return 0;
}

