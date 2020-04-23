#include <FrameCollector.h>
#include <common.h>
#include <logging.h>
#include <V4l2.h>

FrameCollector::FrameCollector( int device = 0 )
{

   capture.reset( new V4l2( "/dev/video" + std::to_string( device ) ) );
   height = VRES;
   width  = HRES;
   // windowName = "FRAME_CAPTURE";
   // cv::namedWindow( windowName, CV_WINDOW_AUTOSIZE );
   thread.reset( new CyclicThread( captureThreadConfig, FrameCollector::execute, this, true ) );
}

FrameCollector::~FrameCollector()
{
}

void FrameCollector::terminate()
{
   capture->stopCapture();
   thread->terminate();
}

void* FrameCollector::execute( void* args )
{
   static FrameCollector* fc = &getCollector();

   unsigned int count = 0;
   struct timespec read_delay;
   struct timespec time_error;

   read_delay.tv_sec  = 0;
   read_delay.tv_nsec = 30000;

   if ( fc->frameCount < FRAMES_TO_EXECUTE )
   {
      if ( fc->capture->readFrame() )
      {
         if ( nanosleep( &read_delay, &time_error ) != 0 )
         {
            perror( "nanosleep" );
         }
         else
         {
            printf( "time_error.tv_sec=%ld, time_error.tv_nsec=%ld\n", time_error.tv_sec, time_error.tv_nsec );
         }
         fc->frameCount++;
      }
   }
}
