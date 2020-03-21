#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

const char* window_name[] = {
    "Edge Detector Transform",
    "Hough Line Transform",
    "Hough Elliptical Transform"};

int lowThreshold           = 0;
int const max_lowThreshold = 100;
int kernel_size            = 3;
int edgeThresh             = 1;
int ratio                  = 3;
cv::Mat canny_frame, cdst, timg_gray, timg_grad;

IplImage* frame;

bool doCanny;
bool doHoughLine;
bool doHoughElliptical;

void CannyThreshold( int, void* )
{
   //Mat mat_frame(frame);
   cv::Mat mat_frame( cv::cvarrToMat( frame ) );

   cv::cvtColor( mat_frame, timg_gray, CV_RGB2GRAY );

   /// Reduce noise with a kernel 3x3
   cv::blur( timg_gray, canny_frame, cv::Size( 3, 3 ) );

   /// Canny detector
   cv::Canny( canny_frame, canny_frame, lowThreshold, lowThreshold * ratio, kernel_size );

   /// Using Canny's output as a mask, we display our result
   timg_grad = cv::Scalar::all( 0 );

   mat_frame.copyTo( timg_grad, canny_frame );

   cv::imshow( window_name[ 0 ], timg_grad );
}

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

   if ( deviceName )
   {
      struct stat buffer;
      if ( 0 == stat( deviceName, &buffer ) )
      {
         printf( "exists!\n" );
      }
      else
      {
         printf( "does not exist!\n" );
      }
   }

   return 0;
}