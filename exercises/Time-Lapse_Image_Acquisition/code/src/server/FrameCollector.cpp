#include <FrameCollector.h>
#include <common.h>
#include <logging.h>
#include <V4l2.h>

FrameCollector::FrameCollector( int device = 0 )
{

   capture.reset( new V4l2( "/dev/video" + std::to_string( device ) ) );
   // capture->open( device );
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
}

void* FrameCollector::execute( void* args )
{
   static FrameCollector* fc = &getCollector();

   if ( fc->frameCount < FRAMES_TO_EXECUTE )
   {
      // if ( fc->capture->isOpened() )
      // {
         // fc->capture->read( fc->frame );
      // }
      // if ( fc->frame.empty() )
      // {
         // return NULL;
      // }
      // cv::imshow( fc->windowName, fc->frame );

      // sprintf( &snapshotname.front(), "snapshot_%d.ppm", fc->frameCount );
      // cv::imwrite( snapshotname, fc->frame );
      fc->frameCount++;
      // cv::waitKey( 1 );
   }
}
