#include <logging.h>
#include <signal.h>
#include <sockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <thread.h>
#include <unistd.h>
#include <frameCapture.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


std::string WindowSize;

pthread_mutex_t captureLock;
pthread_mutex_t windowLock;
// CvCapture* capture;
// IplImage* frame;

// static std::string snapshotname = "snapshot_xxx.ppm";

// void* doCapture( void* args )
// {
//    cvNamedWindow( "Capture Example", CV_WINDOW_AUTOSIZE );

//    int count = 0;
//    while ( count < FRAMES_TO_EXECUTE )
//    {
//       frame = cvQueryFrame( capture );
//       if ( !frame )
//       {
//          logging::ERROR( "Invalid frame", true );
//          break;
//       }
//       cv::Mat mat_frame( cv::cvarrToMat( frame ) );
//       cv::imshow( "Capture Example", mat_frame );

//       sprintf( &snapshotname.front(), "snapshot_%d.ppm", count );
//       cv::imwrite( snapshotname.c_str(), mat_frame );
//       count++;
//       char c = cvWaitKey( 1 );
//       if ( c == 'q' )
//       {
//          break;
//       }
//    }
//    cvReleaseCapture( &capture );
//    cvDestroyWindow( "Capture Example" );
//    return NULL;
// }

// const ProcessParams captureProcessParams = {
//     CPU_MAIN,  // CPU1
//     SCHED_FIFO,
//     2,  // low priority
//     0};

// const ThreadConfigData captureThreadConfig = {
//     true,
//     "capture",
//     captureProcessParams};

int main( int argc, char* argv[] )
{
   const char* host = getCmdOption( argv, argv + argc, "--host" );
   if ( !host )
   {
      host = sockets::LOCALHOST;
   }

   pid_t mainThreadId       = getpid();
   std::string fileName     = "server" + std::to_string( mainThreadId ) + ".log";
   logging::config_s config = {logging::LogLevel::TRACE, fileName};
   logging::configure( config );
   logging::INFO( "SERVER ON " + std::string( host ), true );

   printf( "SERVER HERE!\n" );

   FrameCapture* frameCapture = &getFrameCapture(0);

   while( frameCapture->frameCount < FRAMES_TO_EXECUTE )
   {
      logging::INFO( "FRAME # " + std::to_string( frameCapture->frameCount ), true );
   }
   frameCapture->terminate();
   delete frameCapture;
   //delete frameCapture;
   int client           = -1;

   //SocketServer* server = new SocketServer( "192.168.137.41", DEFAULTPORT );
   sockets::SocketServer* server = new sockets::SocketServer( host, sockets::DEFAULTPORT );

   logging::message_s* serverMessage = new logging::message_s;
   serverMessage->ThreadID           = THREAD_SERVER;
   serverMessage->level              = logging::LogLevel::TRACE;
   sprintf( serverMessage->msg, "test server" );

   logging::log( serverMessage );
   server->listen( 10 );

   while ( 0 > client )
   {
      logging::log( serverMessage );
      client = server->accept();
   }

   const char* patterns[] = {
       "Hello World",
       "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
       "12345671231243",
       "Another message!",
       "One more!"};

   for ( int i = 0; i < 5; i++ )
   {
      logging::log( serverMessage );

      server->send( client, patterns[ i ] );
      logging::log( serverMessage );

      server->read( client );
   }

   delete server;
   return 0;
}
