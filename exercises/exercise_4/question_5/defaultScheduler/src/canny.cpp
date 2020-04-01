#include "canny.h"

#include "common.h"
#include "logging.h"

static IplImage* frame;
static cv::Mat canny_frame, cdst, timg_gray, timg_grad;

static const logging::message_s start = {
    logging::LogLevel::TRACE,
    THREAD_CANNY,
    false,
    "CANNY START"};

static const logging::message_s end = {
    logging::LogLevel::TRACE,
    THREAD_CANNY,
    true,
    "CANNY END  "};

int kernel_size = 3;
int edgeThresh  = 1;
int ratio       = 3;

extern CvCapture* capture;

void CannyThreshold( int, void* )
{
   logging::log( &start );
   cv::Mat mat_frame( cv::cvarrToMat( frame ) );

   cv::cvtColor( mat_frame, timg_gray, CV_RGB2GRAY );

   /// Reduce noise with a kernel 3x3
   cv::blur( timg_gray, canny_frame, cv::Size( 3, 3 ) );

   /// Canny detector
   cv::Canny( canny_frame, canny_frame, lowThreshold, lowThreshold * ratio, kernel_size );

   /// Using Canny's output as a mask, we display our result
   timg_grad = cv::Scalar::all( 0 );

   mat_frame.copyTo( timg_grad, canny_frame );

#ifdef SHOW_WINDOWS
   pthread_mutex_lock( &windowLock );
   cv::imshow( window_name[ THREAD_CANNY ], timg_grad );
   pthread_mutex_unlock( &windowLock );
#endif
   logging::log( &end );
}

void* executeCanny( void* args )
{
   logging::INFO( "executeCanny entered!", true );

   uint32_t frame_count = 0;

   while ( frame_count < FRAMES_TO_EXECUTE and false == isTimeToDie )
   {
      frame_count++;
      pthread_mutex_lock( &captureLock );
      frame = cvQueryFrame( capture );
      pthread_mutex_unlock( &captureLock );

      if ( !frame )
         break;

      CannyThreshold( 0, 0 );

      char q = cvWaitKey( 1 );
      if ( q == 'q' )
      {
         printf( "got quit\n" );
         break;
      }
   }

   logging::INFO( "executeCanny exiting!", true );



   return NULL;
}
