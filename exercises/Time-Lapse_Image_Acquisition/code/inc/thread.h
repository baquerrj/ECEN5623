#ifndef __THREAD_H__
#define __THREAD_H__

#include <thread_utils.h>

class ThreadBase
{
public:
   virtual ~ThreadBase() = 0;

   virtual void setFunctionAndOwner( void *( *execute_ )( void *context ),
                                     void *owner_ ) = 0;
   virtual void initiateThread()                    = 0;
   virtual void terminate()                         = 0;

protected:
   ThreadConfigData threadData;
   pthread_t thread;
   bool threadIsAlive;
   void *( *execute )( void *context );
   void *owner;
};

class CyclicThread : public ThreadBase
{
public:
   CyclicThread( const ThreadConfigData &configData,
                 void *( *execute_ )( void *context ),
                 void *owner_,
                 bool readyForThread = true );
   CyclicThread( const ThreadConfigData &configData );
   virtual ~CyclicThread();

   virtual void setFunctionAndOwner( void *( *execute_ )( void *context ),
                                     void *owner_ );
   virtual void initiateThread();
   virtual void terminate();

   pthread_t getThreadId();
protected:
   void *( *execute )( void *context );
   virtual void *cycle();
   static void *threadFunction( void *context );

   void *owner;
};

inline pthread_t CyclicThread::getThreadId()
{
   return thread;
}

#endif  // __THREAD_H__
