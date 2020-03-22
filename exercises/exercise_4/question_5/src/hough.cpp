#include "hough.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "common.h"

static IplImage* frame;
extern int width;
extern int height;

static cv::Mat gray, canny_frame, cdst;
std::vector< cv::Vec4i > lines;

void HoughLines( int, void* )
{
   cv::Mat mat_frame( cv::cvarrToMat( frame ) );
   cv::Canny( mat_frame, canny_frame, 50, 200, 3 );

   cv::cvtColor( canny_frame, cdst, CV_GRAY2BGR );
   cv::cvtColor( mat_frame, gray, CV_BGR2GRAY );

   cv::HoughLinesP( canny_frame, lines, 1, CV_PI / 180, 50, 50, 10 );

   for ( size_t i = 0; i < lines.size(); i++ )
   {
      cv::Vec4i l = lines[ i ];
      cv::line( mat_frame,
                cv::Point( l[ 0 ], l[ 1 ] ),
                cv::Point( l[ 2 ], l[ 3 ] ),
                cv::Scalar( 0, 0, 255 ), 3, CV_AA );
   }

   cv::imshow( window_name[ 1 ], mat_frame );
}

void* executeHough( void* args )
{
   int dev = 0;
   if( NULL != args )
   {
      int dev = *((int *) args);
   }
   CvCapture* capture;

   cv::namedWindow( window_name[ 1 ], CV_WINDOW_AUTOSIZE );

   capture = (CvCapture*)cvCreateCameraCapture( dev );
   cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, width );
   cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, height );

   while ( 1 )
   {
      frame = cvQueryFrame( capture );
      if ( !frame )
         break;

      HoughLines( 0, 0 );

      char c = cvWaitKey( 10 );
      if ( c == 'q' )
      {
         printf( "got quit\n" );
         break;
      }
   }
}