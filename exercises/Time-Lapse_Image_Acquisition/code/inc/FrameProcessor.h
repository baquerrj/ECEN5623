#ifndef __FRAME_PROCESSOR_H__
#define __FRAME_PROCESSOR_H__

#include <FrameBase.h>
#include <V4l2.h>
#include <time.h>

class CyclicThread;

class FrameProcessor : public FrameBase
{
public:
   FrameProcessor();
   ~FrameProcessor();

   void readFrame();
   int processImage( const void* p, int size );
   int processImage( V4l2::buffer_s* img );
   void dumpImage( const void* p, int size, unsigned int tag, struct timespec* time );

   void terminate();
   static void* execute( void* context );

private:
   void yuv2rgb( int y, int u, int v, unsigned char* r, unsigned char* g, unsigned char* b );
};

#endif  // __FRAME_PROCESSOR_H__