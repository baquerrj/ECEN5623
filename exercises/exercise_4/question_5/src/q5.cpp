#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

static const uint32_t HRES = 640;
static const uint32_t VRES = 480;

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

bool doCanny = false;
bool doHoughLine = false;
bool doHoughElliptical = false;

int width = HRES;
int height = VRES;

void* executeCanny( int8_t dev );
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

void* executeCanny( int8_t dev )
{
   CvCapture* canny_capture;

   cv::namedWindow( window_name[ 0 ], CV_WINDOW_AUTOSIZE );
   // Create a Trackbar for user to enter threshold
   cv::createTrackbar( "Min Threshold:", window_name[ 0 ], &lowThreshold, max_lowThreshold, CannyThreshold );

   canny_capture = (CvCapture*)cvCreateCameraCapture( dev );
   cvSetCaptureProperty( canny_capture, CV_CAP_PROP_FRAME_WIDTH, width );
   cvSetCaptureProperty( canny_capture, CV_CAP_PROP_FRAME_HEIGHT, height );
   
   while ( 1 )
   {
      frame = cvQueryFrame( canny_capture );
      if ( !frame )
         break;

      CannyThreshold( 0, 0 );

      char q = cvWaitKey( 33 );
      if ( q == 'q' )
      {
         printf( "got quit\n" );
         break;
      }
   }
}
