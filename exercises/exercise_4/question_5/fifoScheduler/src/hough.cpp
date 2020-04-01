#include "hough.h"

#include "common.h"
#include "logging.h"

static IplImage* lFrame;
static cv::Mat lGray, lCannyFrame, lCdst;
static std::vector< cv::Vec4i > lines;

static const logging::message_s start = {
    logging::LogLevel::TRACE,
    THREAD_HOUGHL,
    false,
    "HOUGH-LINES START"};

static const logging::message_s end = {
    logging::LogLevel::TRACE,
    THREAD_HOUGHL,
    true,
    "HOUGH-LINES END  "};

extern CvCapture* capture;

void HoughLines( int, void* )
{
   logging::log( &start );
   cv::Mat lMatFrame( cv::cvarrToMat( lFrame ) );
   cv::Canny( lMatFrame, lCannyFrame, 50, 200, 3 );

   cv::cvtColor( lCannyFrame, lCdst, CV_GRAY2BGR );
   cv::cvtColor( lMatFrame, lGray, CV_BGR2GRAY );

   cv::HoughLinesP( lCannyFrame, lines, 1, CV_PI / 180, 50, 50, 10 );

   for ( size_t i = 0; i < lines.size(); i++ )
   {
      cv::Vec4i l = lines[ i ];
      cv::line( lMatFrame,
                cv::Point( l[ 0 ], l[ 1 ] ),
                cv::Point( l[ 2 ], l[ 3 ] ),
                cv::Scalar( 0, 0, 255 ), 3, CV_AA );
   }

#ifdef SHOW_WINDOWS
   pthread_mutex_lock( &windowLock );
   cv::imshow( window_name[ THREAD_HOUGHL ], lMatFrame );
   pthread_mutex_unlock( &windowLock );
#endif
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
         lFrame = cvQueryFrame( capture );
         pthread_mutex_unlock( &captureLock );
         if ( !lFrame )
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