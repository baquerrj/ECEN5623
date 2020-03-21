#include "hough.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "common.h"

extern IplImage* frame;
extern int width;
extern int height;

static cv::Mat gray;
std::vector< cv::Vec3f > circles;

void HoughElliptical( int, void* )
{
   cv::Mat mat_frame( cv::cvarrToMat( frame ) );

   cv::cvtColor( mat_frame, gray, CV_BGR2GRAY );

   cv::GaussianBlur( gray, gray, cv::Size( 9, 9 ), 2, 2 );

   cv::HoughCircles( gray, circles, CV_HOUGH_GRADIENT, 1, gray.rows / 8, 100, 50, 0, 0 );

   for ( size_t i = 0; i < circles.size(); i++ )
   {
      cv::Point center( cvRound( circles[ i ][ 0 ] ), cvRound( circles[ i ][ 1 ] ) );
      int radius = cvRound( circles[ i ][ 2 ] );
      // circle center
      cv::circle( mat_frame, center, 3, cv::Scalar( 0, 255, 0 ), -1, 8, 0 );
      // circle outline
      cv::circle( mat_frame, center, radius, cv::Scalar( 0, 0, 255 ), 3, 8, 0 );
   }

   cv::imshow( window_name[ 2 ], mat_frame );
}

void* executeHoughElliptical( int dev )
{
   CvCapture* capture;

   cv::namedWindow( window_name[ 2 ], CV_WINDOW_AUTOSIZE );

   capture = (CvCapture*)cvCreateCameraCapture( dev );
   cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, width );
   cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, height );

   while ( 1 )
   {
      frame = cvQueryFrame( capture );
      if ( !frame )
         break;

      HoughElliptical( 0, 0 );

      char c = cvWaitKey( 10 );
      if ( c == 'q' )
      {
         printf( "got quit\n" );
         break;
      }
   }
}