#ifndef __FRAMECOLLECTOR_H__
#define __FRAMECOLLECTOR_H__

#include <FrameBase.h>

class CyclicThread;
class V4l2;

class FrameCollector : public FrameBase
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
   // std::string name;
   // double wcet;
   // double aet;
   // unsigned long long count;
   // uint32_t frameCount;
   // double diff_time;       //!< To store execution time for each iteration
   // struct timespec start;  //!< To measure start time of the service
   // struct timespec end;    //!< To measure end time of the service

   // bool isAlive;
   // sem_t sem;
   V4l2* capture;
   // CyclicThread* thread;

   // double* executionTimes;  //!< To store execution time for each iteration
   // double* startTimes;      //!< To store start time for each iteration
   // double* endTimes;        //!< To store end time for each iteration
};

// inline sem_t* FrameCollector::getSemaphore( void )
// {
//    return &sem;
// }

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