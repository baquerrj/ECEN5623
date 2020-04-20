#include <FrameCollector.h>
#include <common.h>
#include <logging.h>

FrameCollector::FrameCollector( int device = 0 )
{
   capture = new cv::VideoCapture();
   capture->open( device );
   height = VRES;
   width  = HRES;
   // cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, width );
   // cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, height );
   windowName = "FRAME_CAPTURE";
   cv::namedWindow( windowName, CV_WINDOW_AUTOSIZE );
   thread.reset( new CyclicThread( captureThreadConfig, FrameCollector::execute, this, true ) );
}

FrameCollector::~FrameCollector()
{
   logging::DEBUG( "FrameCollector::~FrameCollector() entered", true );
   capture->release();
   cvDestroyWindow( windowName.c_str() );
   logging::DEBUG( "FrameCollector::~FrameCollector() exiting", true );
}

void FrameCollector::terminate()
{
}

void* FrameCollector::execute( void* args )
{
   static FrameCollector* fc = &getCollector();

   if ( fc->frameCount < FRAMES_TO_EXECUTE )
   {
      if ( fc->capture->isOpened() )
      {
         fc->capture->read( fc->frame );
      }
      if ( fc->frame.empty() )
      {
         return NULL;
      }
      cv::imshow( fc->windowName, fc->frame );

      sprintf( &snapshotname.front(), "snapshot_%d.ppm", fc->frameCount );
      cv::imwrite( snapshotname.c_str(), fc->frame );
      fc->frameCount++;
      cv::waitKey( 1 );
   }
}
