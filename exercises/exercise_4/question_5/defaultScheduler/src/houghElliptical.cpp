#include "houghElliptical.h"

#include "common.h"
#include "logging.h"

static IplImage* eFrame;
static cv::Mat eGray;
static std::vector< cv::Vec3f > circles;

static const logging::message_s start = {
    logging::LogLevel::TRACE,
    THREAD_HOUGHE,
    false,
    "HOUGH-ELLIP START"};

static const logging::message_s end = {
    logging::LogLevel::TRACE,
    THREAD_HOUGHE,
    true,
    "HOUGH-ELLIP END  "};

extern CvCapture* capture;

void HoughElliptical( int, void* )
{
   logging::log( &start );
   cv::Mat eMatFrame( cv::cvarrToMat( eFrame ) );

   cv::cvtColor( eMatFrame, eGray, CV_BGR2GRAY );

   cv::GaussianBlur( eGray, eGray, cv::Size( 9, 9 ), 2, 2 );

   cv::HoughCircles( eGray, circles, CV_HOUGH_GRADIENT, 1, eGray.rows / 8, 100, 50, 0, 0 );

   for ( size_t i = 0; i < circles.size(); i++ )
   {
      cv::Point center( cvRound( circles[ i ][ 0 ] ), cvRound( circles[ i ][ 1 ] ) );
      int radius = cvRound( circles[ i ][ 2 ] );
      // circle center
      cv::circle( eMatFrame, center, 3, cv::Scalar( 0, 255, 0 ), -1, 8, 0 );
      // circle outline
      cv::circle( eMatFrame, center, radius, cv::Scalar( 0, 0, 255 ), 3, 8, 0 );
   }
#ifdef SHOW_WINDOWS
   pthread_mutex_lock( &windowLock );
   cv::imshow( window_name[ THREAD_HOUGHE ], eMatFrame );
   pthread_mutex_unlock( &windowLock );
#endif
   logging::log( &end );
}

void* executeHoughElliptical( void* args )
{
   logging::INFO( "executeHoughElliptical entered!", true );
   uint32_t frame_count = 0;

   while ( frame_count < FRAMES_TO_EXECUTE and false == isTimeToDie )
   {
      frame_count++;
      pthread_mutex_lock( &captureLock );
      eFrame = cvQueryFrame( capture );
      pthread_mutex_unlock( &captureLock );
      if ( !eFrame )
         break;

      HoughElliptical( 0, 0 );

      char c = cvWaitKey( 1 );
      if ( c == 'q' )
      {
         printf( "got quit\n" );
         break;
      }
   }

   logging::INFO( "executeHoughElliptical exiting!", true );

   return NULL;
}