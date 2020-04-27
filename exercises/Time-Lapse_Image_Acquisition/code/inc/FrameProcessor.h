#ifndef __FRAME_PROCESSOR_H__
#define __FRAME_PROCESSOR_H__

#include <semaphore.h>
#include <time.h>

#include <memory>

class CyclicThread;

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

   sem_t* getSemaphore( void );

private:
   void yuv2rgb( int y, int u, int v, unsigned char* r, unsigned char* g, unsigned char* b );

private:
   sem_t sem;
   std::string name;
   CyclicThread* thread;

   double wcet;
   double aet;
   double* executionTimes;  //!< To store execution time for each iteration
   double* startTimes;      //!< To store start time for each iteration
   double* endTimes;        //!< To store end time for each iteration

   struct timespec start;  //!< To measure start time of the service
   struct timespec end;    //!< To measure end time of the service
   double diff_time;               //!< To store execution time for each iteration

   unsigned long long S2Cnt;
};

inline sem_t* FrameProcessor::getSemaphore( void )
{
   return &sem;
}
#endif  // __FRAME_PROCESSOR_H__