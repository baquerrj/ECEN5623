#ifndef __FRAMECAPTURE_H__
#define __FRAMECAPTURE_H__

#include <thread.h>
#include <thread_utils.h>

#include <memory>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

static std::string snapshotname = "snapshot_xxx.ppm";

static const ProcessParams captureProcessParams = {
    CPU_MAIN,  // CPU1
    SCHED_FIFO,
    2,  // low priority
    0};

static const ThreadConfigData captureThreadConfig = {
    true,
    "capture",
    captureProcessParams};

class FrameCapture
{
public:
   FrameCapture( int device );
   FrameCapture( const FrameCapture& fc );
   ~FrameCapture();

   void terminate();
   static void* execute( void* args );

   uint32_t frameCount;

private:
   std::string windowName;
   CvCapture* capture;
   IplImage* frame;
   cv::Mat* matFrame;

   uint16_t height;
   uint16_t width;
   std::unique_ptr< CyclicThread > thread;
};

inline FrameCapture& getFrameCapture( int device = 0 )
{
   static std::unique_ptr< FrameCapture > singleton( new FrameCapture( device ) );
   return *singleton;
}

#endif  // __FRAMECAPTURE_H__