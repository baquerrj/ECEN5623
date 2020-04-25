#ifndef __FRAMECOLLECTOR_H__
#define __FRAMECOLLECTOR_H__

#include <thread.h>
#include <thread_utils.h>

#include <memory>

class V4l2;

static const ProcessParams collectorParams = {
    cpuMain,  // CPU1
    SCHED_FIFO,
    99,  // highest priority
    0};

static const ThreadConfigData collectorThreadConfig = {
    true,
    "COLLECTOR",
    collectorParams};

class FrameCollector
{
public:
   FrameCollector( int device );
   FrameCollector( const FrameCollector& fc );
   ~FrameCollector();

   void terminate();
   void collectFrame( void );
   static void* execute( void* context );

   uint32_t getFrameCount( void );
private:
   V4l2* capture;
   CyclicThread* thread;
   uint32_t frameCount;
};

inline uint32_t FrameCollector::getFrameCount( void )
{
   return frameCount;
}

inline FrameCollector& getCollector( int device = 0 )
{
   static std::unique_ptr< FrameCollector > singleton( new FrameCollector( device ) );
   return *singleton;
}

#endif  // __FRAMECOLLECTOR_H__