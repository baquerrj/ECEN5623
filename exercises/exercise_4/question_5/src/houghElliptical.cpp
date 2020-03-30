#include "common.h"
#include "hough.h"
#include "logging.h"

static IplImage* frame;
static cv::Mat gray;
static std::vector< cv::Vec3f > circles;

static const logging::message_s start = {
    logging::LogLevel::TRACE,
    "HOUGH-ELLIP START"};

static const logging::message_s end = {
    logging::LogLevel::TRACE,
    "HOUGH-ELLIP END"};

extern CvCapture* capture;

void HoughElliptical( int, void* )
{
   logging::log( &start );
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

   pthread_mutex_lock( &windowLock );
   cv::imshow( window_name[ THREAD_HOUGHE ], mat_frame );
   pthread_mutex_unlock( &windowLock );
   logging::log( &end );
}

void* executeHoughElliptical( void* args )
{
   uint32_t frame_count = 0;

   while ( false == isTimeToDie )
   {
      //semWait( THREAD_HOUGHE );

      while ( frame_count < FRAMES_TO_EXECUTE and false == isTimeToDie )
      {
         frame_count++;
         pthread_mutex_lock( &captureLock );
         frame = cvQueryFrame( capture );
         pthread_mutex_unlock( &captureLock );
         if ( !frame )
            break;

         HoughElliptical( 0, 0 );

         char c = cvWaitKey( 1 );
         if ( c == 'q' )
         {
            printf( "got quit\n" );
            break;
         }
      }

      break;
   }

   return NULL;
}