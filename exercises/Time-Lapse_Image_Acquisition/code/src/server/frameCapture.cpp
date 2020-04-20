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
   capture = (CvCapture*)cvCreateCameraCapture( device );
   height  = VRES;
   width   = HRES;
   cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, width );
   cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, height );
   windowName = "FRAME_CAPTURE";
   cv::namedWindow( windowName, CV_WINDOW_AUTOSIZE );
   thread.reset( new CyclicThread( captureThreadConfig, FrameCapture::execute, this, true ) );
}

FrameCapture::~FrameCapture()
{
   // if ( NULL != capture )
   // {
      // cvReleaseCapture( &capture );
      // logging::DEBUG( "windowName is " + windowName );
      // cvDestroyWindow( windowName.c_str() );
      // capture = NULL;
   // }
   // thread->terminate();
}

void FrameCapture::terminate()
{
   logging::DEBUG( "FrameCapture::terminate() entered", true );
   if ( NULL != capture )
   {
      cvReleaseCapture( &capture );
      cvDestroyWindow( windowName.c_str() );
      capture = NULL;
   }
   thread->terminate();
   logging::DEBUG( "FrameCapture::terminate() exiting", true );
}

void* FrameCapture::execute( void* args )
{
   static FrameCapture* f = &getFrameCapture();

   if ( f->frameCount < FRAMES_TO_EXECUTE )
   {
      f->frame = cvQueryFrame( f->capture );
      if ( !f->frame )
      {
         return NULL;
      }
      cv::Mat mat_frame( cv::cvarrToMat( f->frame ) );
      cv::imshow( f->windowName, mat_frame );

      sprintf( &snapshotname.front(), "snapshot_%d.ppm", f->frameCount );
      cv::imwrite( snapshotname.c_str(), mat_frame );
      f->frameCount++;
      char c = cvWaitKey( 1 );
      // if ( c == 'q' )
      // {
      //    break;
      // }
   }
}
