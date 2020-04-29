#ifndef __FRAME_BASE_H__
#define __FRAME_BASE_H__

#include <semaphore.h>
#include <string.h>

#include <memory>

class CyclicThread;
struct ThreadConfigData;

class FrameBase
{
public:
   FrameBase( const ThreadConfigData config );
   virtual ~FrameBase();

   virtual void shutdown();
   virtual bool isAlive();
   virtual bool isThreadAlive();
   virtual pthread_t getThreadId();
   virtual sem_t* getSemaphore();
   virtual void jitterAnalysis();

   virtual void setDeadline( double deadlineTime );
protected:
   std::string name;
   double wcet;
   double aet;
   double deadline;
   unsigned long long count;
   uint32_t frameCount;
   double diff_time;       //!< To store execution time for each iteration
   struct timespec start;  //!< To measure start time of the service
   struct timespec end;    //!< To measure end time of the service

   double* executionTimes;  //!< To store execution time for each iteration
   double* startTimes;      //!< To store start time for each iteration
   double* endTimes;        //!< To store end time for each iteration

   bool alive;
   sem_t sem;
   CyclicThread* thread;
};

#endif  //__FRAME_BASE_H__
