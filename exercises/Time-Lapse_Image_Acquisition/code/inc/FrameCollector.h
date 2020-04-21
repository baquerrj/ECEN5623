#ifndef __FRAMECOLLECTOR_H__
#define __FRAMECOLLECTOR_H__

#include <thread.h>
#include <thread_utils.h>

#include <memory>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

static std::string snapshotname = "snapshot_xxx.ppm";

static const ProcessParams captureProcessParams = {
    cpuMain,  // CPU1
    SCHED_FIFO,
    99,  // highest priority
    0};

static const ThreadConfigData captureThreadConfig = {
    true,
    "capture",
    captureProcessParams};

class FrameCollector
{
public:
   FrameCollector( int device );
   FrameCollector( const FrameCollector& fc );
   ~FrameCollector();

   void terminate();
   static void* execute( void* args );

   uint32_t frameCount;

private:
   std::string windowName;
   std::unique_ptr< cv::VideoCapture > capture;
   cv::Mat frame;

   uint16_t height;
   uint16_t width;
   std::unique_ptr< CyclicThread > thread;
};

inline FrameCollector& getCollector( int device = 0 )
{
   static std::unique_ptr< FrameCollector > singleton( new FrameCollector( device ) );
   return *singleton;
}

#endif  // __FRAMECOLLECTOR_H__