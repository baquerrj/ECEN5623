#ifndef __FRAME_PROCESSOR_H__
#define __FRAME_PROCESSOR_H__

#include <time.h>

class FrameProcessor
{
public:
   FrameProcessor();
   ~FrameProcessor();

   int readFrame();
   int processImage( const void* p, int size );
   void dumpImage( const void* p, int size, unsigned int tag, struct timespec* time );

private:
   void yuv2rgb( int y, int u, int v, unsigned char* r, unsigned char* g, unsigned char* b );
};

#endif  // __FRAME_PROCESSOR_H__