#include "common.h"
#include "hough.h"
#include "logging.h"

static IplImage* frame;
static cv::Mat gray, canny_frame, cdst, mat_frame;
static std::vector< cv::Vec4i > lines;

static const logging::message_s start = {
    logging::LogLevel::TRACE,
    "HOUGH-LINES START"};

static const logging::message_s end = {
    logging::LogLevel::TRACE,
    "HOUGH-LINES END"};

extern CvCapture* capture;

void HoughLines( int, void* )
{
   logging::log( &start );
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

   pthread_mutex_lock( &windowLock );
   cv::imshow( window_name[ THREAD_HOUGHL ], mat_frame );
   pthread_mutex_unlock( &windowLock );
   logging::log( &end );
}

void* executeHough( void* args )
{
   uint32_t frame_count = 0;

   while ( false == isTimeToDie )
   {
      //semWait( THREAD_HOUGHL );

      while ( frame_count < FRAMES_TO_EXECUTE and false == isTimeToDie )
      {
         frame_count++;
         pthread_mutex_lock( &captureLock );
         frame = cvQueryFrame( capture );
         pthread_mutex_unlock( &captureLock );
         if ( !frame )
            break;

         HoughLines( 0, 0 );
         char c = cvWaitKey( 1 );
         if ( c == 'q' )
         {
            printf( "got quit\n" );
            break;
         }
      }

      //semPost( THREAD_HOUGHE );
      break;
   }

   return NULL;
}