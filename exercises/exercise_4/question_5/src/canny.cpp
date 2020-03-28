#include "canny.h"

#include <signal.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "common.h"
#include "logging.h"

static IplImage* frame;
static cv::Mat canny_frame, cdst, timg_gray, timg_grad;
static int lowThreshold           = 0;
static const int max_lowThreshold = 100;

static const logging::message_s start = {
    logging::log_level::INFO,
    "CANNY START"};

static const logging::message_s end = {
    logging::log_level::INFO,
    "CANNY END"};

int kernel_size = 3;
int edgeThresh  = 1;
int ratio       = 3;

extern CvCapture* capture;

static void signalHandler( int signo )
{
   switch ( signo )
   {
      case SIGINT:
      {
         int exit = 0;
         pthread_exit( &exit );
         break;
      }
      default:
      {
         logging::INFO( "Unknown signal caught!" );
         break;
      }
   }
}

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
   cv::imshow( window_name[ 0 ], timg_grad );
   pthread_mutex_unlock( &windowLock );
#endif
   logging::log( &end );
}

void* executeCanny( void* args )
{
   uint16_t frame_count = 0;
   //float frame_rate;
   //struct timespec start_time, stop_time, diff_time;
   signal( SIGINT, signalHandler );
   int dev = 0;
   if ( NULL != args )
   {
      int dev = *( (int*)args );
   }
   //CvCapture* capture;

#ifdef SHOW_WINDOWS
   cv::namedWindow( window_name[ THREAD_CANNY ], CV_WINDOW_AUTOSIZE );
   // Create a Trackbar for user to enter threshold
   cv::createTrackbar( "Min Threshold:", window_name[ THREAD_CANNY ], &lowThreshold, max_lowThreshold, CannyThreshold );
#endif

   //capture = (CvCapture*)cvCreateCameraCapture( dev );
   //cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, width );
   //cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, height );

   while ( false == isTimeToDie )
   {
      semWait( THREAD_CANNY );

      //clock_gettime(CLOCK_REALTIME, &start_time);
      while ( frame_count < 50 and false == isTimeToDie )
      {
         frame_count++;

         pthread_mutex_lock( &captureLock );
         frame = cvQueryFrame( capture );
         pthread_mutex_unlock( &captureLock );

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
      //clock_gettime(CLOCK_REALTIME, &stop_time);
      //delta_t(&stop_time, &start_time, &diff_time);
      //frame_rate = (float)frame_count/((diff_time.tv_sec * NSEC_PER_SEC + diff_time.tv_nsec) / NSEC_PER_SEC );
      //printf("Frame Rate of Canny Edge Detection is %f\n",frame_rate);

#ifdef SHOW_WINDOWS
      cvDestroyWindow( window_name[ THREAD_CANNY ] );
#endif

      semPost( THREAD_HOUGHL );

      break;
   }

   return NULL;
}
