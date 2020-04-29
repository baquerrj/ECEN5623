#ifndef __FRAME_BASE_H__
#define __FRAME_BASE_H__

#include <string.h>
#include <semaphore.h>
#include <memory>

class CyclicThread;

class FrameBase
{
public:
   FrameBase();
   virtual ~FrameBase();

   virtual void shutdown();
   virtual bool isAlive();
   virtual bool isThreadAlive();
   virtual pthread_t getThreadId();
   virtual sem_t* getSemaphore( void );

protected:
   std::string name;
   bool alive;
   sem_t sem;
   CyclicThread* thread;
};

#endif //__FRAME_BASE_H__
