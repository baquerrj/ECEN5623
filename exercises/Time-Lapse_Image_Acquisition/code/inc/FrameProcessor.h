#ifndef __FRAME_PROCESSOR_H__
#define __FRAME_PROCESSOR_H__

#include <FrameBase.h>
#include <time.h>
#include <V4l2.h>

class CyclicThread;

class FrameProcessor : public FrameBase
{
public:
   FrameProcessor();
   ~FrameProcessor();

   int readFrame();
   int processImage( const void* p, int size );
   int processImage( V4l2::buffer_s* img );
   void dumpImage( const void* p, int size, unsigned int tag, struct timespec* time );

   void terminate();
   static void* execute( void* context );

   sem_t* getSemaphore( void );

private:
   void yuv2rgb( int y, int u, int v, unsigned char* r, unsigned char* g, unsigned char* b );

private:
   // std::string name;
   double wcet;
   double aet;
   unsigned long long count;
   uint32_t frameCount;
   double diff_time;       //!< To store execution time for each iteration
   struct timespec start;  //!< To measure start time of the service
   struct timespec end;    //!< To measure end time of the service

   // bool isAlive;
   // sem_t sem;
   // CyclicThread* thread;

   double* executionTimes;  //!< To store execution time for each iteration
   double* startTimes;      //!< To store start time for each iteration
   double* endTimes;        //!< To store end time for each iteration
};

inline sem_t* FrameProcessor::getSemaphore( void )
{
   return &sem;
}
#endif  // __FRAME_PROCESSOR_H__