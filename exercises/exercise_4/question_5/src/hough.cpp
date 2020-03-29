#include "hough.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "common.h"
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

#ifdef SHOW_WINDOWS
   pthread_mutex_lock( &windowLock );
   cv::imshow( window_name[ THREAD_HOUGHL ], mat_frame );
   pthread_mutex_unlock( &windowLock );
#endif
   logging::log( &end );
}

void* executeHough( void* args )
{
   uint32_t frame_count = 0;
   //float frame_rate;
   //struct timespec start_time, stop_time, diff_time;
   int dev = 0;
   if ( NULL != args )
   {
      int dev = *( (int*)args );
   }

   //capture = (CvCapture*)cvCreateCameraCapture( dev );
   //cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, width );
   //cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, height );

   while ( false == isTimeToDie )
   {
      semWait( THREAD_HOUGHL );

      //clock_gettime( CLOCK_REALTIME, &start_time );
      while ( frame_count < FRAMES_TO_EXECUTE and false == isTimeToDie )
      {
         frame_count++;
         pthread_mutex_lock( &captureLock );
         frame = cvQueryFrame( capture );
         pthread_mutex_unlock( &captureLock );
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
      //clock_gettime(CLOCK_REALTIME, &stop_time);
      //delta_t(&stop_time, &start_time, &diff_time);
      //frame_rate = (float)frame_count/((diff_time.tv_sec * NSEC_PER_SEC + diff_time.tv_nsec) / NSEC_PER_SEC );
      //printf("Frame Rate of Hough Edge Detection is %f\n",frame_rate);

//#ifdef SHOW_WINDOWS
//      cvDestroyWindow( window_name[ THREAD_HOUGHL ] );
//#endif

      semPost( THREAD_HOUGHE );
      break;
   }

   return NULL;
}