#ifndef __FRAME_PROCESSOR_H__
#define __FRAME_PROCESSOR_H__

#include <memory>
#include <time.h>
#include <thread_utils.h>

class CyclicThread;

static const ProcessParams processorParams = {
    cpuMain,  // CPU1
    SCHED_FIFO,
    99,  // highest priority
    0};

static const ThreadConfigData processorThreadConfig = {
    true,
    "PROCESSOR",
    processorParams};

class FrameProcessor
{
public:
   FrameProcessor();
   ~FrameProcessor();

   int readFrame();
   int processImage( const void* p, int size );
   void dumpImage( const void* p, int size, unsigned int tag, struct timespec* time );

   void terminate();
   static void* execute( void* context );
private:
   void yuv2rgb( int y, int u, int v, unsigned char* r, unsigned char* g, unsigned char* b );

private:
   std::string name;
   CyclicThread* thread;
};

#endif  // __FRAME_PROCESSOR_H__