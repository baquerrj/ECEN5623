#include <common.h>
#include <frameCapture.h>
#include <logging.h>

// FrameCapture::FrameCapture( )
// {
//    capture = (CvCapture*)cvCreateCameraCapture( 0 );
//    height  = VRES;
//    width   = HRES;
//    cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, width );
//    cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, height );
//    windowName = "FRAME_CAPTURE";

//    thread.reset( new CyclicThread( captureThreadConfig, FrameCapture::execute, this, true ));
// }

FrameCapture::FrameCapture( int device = 0 )
{
   capture = new cv::VideoCapture();
   capture->open( device );
   height = VRES;
   width  = HRES;
   // cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, width );
   // cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, height );
   windowName = "FRAME_CAPTURE";
   cv::namedWindow( windowName, CV_WINDOW_AUTOSIZE );
   thread.reset( new CyclicThread( captureThreadConfig, FrameCapture::execute, this, true ) );
}

FrameCapture::~FrameCapture()
{
   logging::DEBUG( "FrameCapture::~FrameCapture() entered", true );
   capture->release();
   cvDestroyWindow( windowName.c_str() );
   logging::DEBUG( "FrameCapture::~FrameCapture() exiting", true );
}

void FrameCapture::terminate()
{
}

void* FrameCapture::execute( void* args )
{
   static FrameCapture* f = &getFrameCapture();

   if ( f->frameCount < FRAMES_TO_EXECUTE )
   {
      if ( f->capture->isOpened() )
      {
         f->capture->read( f->frame );
      }
      if ( f->frame.empty() )
      {
         return NULL;
      }
      cv::imshow( f->windowName, f->frame );

      sprintf( &snapshotname.front(), "snapshot_%d.ppm", f->frameCount );
      cv::imwrite( snapshotname.c_str(), f->frame );
      f->frameCount++;
      cv::waitKey( 1 );
   }
}
