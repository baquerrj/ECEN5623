#include <logging.h>
#include <pthread.h>
#include <thread.h>

ThreadBase::~ThreadBase()
{
}

CyclicThread::CyclicThread( const ThreadConfigData &configData,
                            void *( *execute_ )( void *context ),
                            void *owner_,
                            bool readyForThread ) :
    execute( execute_ ),
    owner( owner_ )
{
   threadData    = configData;
   threadIsAlive = false;

   if ( readyForThread )
   {
      initiateThread();
   }
}

CyclicThread::CyclicThread( const ThreadConfigData &configData ) :
    CyclicThread( configData, NULL, NULL, false )
{
}

CyclicThread::~CyclicThread()
{
   terminate();
}

void CyclicThread::setFunctionAndOwner( void *( *execute_ )( void *context ),
                                        void *owner_ )
{
   owner   = owner_;
   execute = execute_;
}

void CyclicThread::initiateThread()
{
   threadIsAlive = true;
   create_thread( threadData.threadName,
                  thread,
                  CyclicThread::threadFunction,
                  this,
                  threadData.processParams );
}

void CyclicThread::terminate()
{
   logging::INFO( "CyclicThread::terminate() entered", true );
   cancel_and_join_thread( thread, threadIsAlive );
   logging::INFO( "CyclicThread::terminate() exiting", true );
}

void *CyclicThread::cycle()
{
   while ( threadIsAlive )
   {
      execute( owner );
   }
   logging::INFO( std::string( "thread shutting down: " + threadData.threadName ), true );
   // pthread_join( thread, NULL );
   pthread_exit( NULL );
   return NULL;
}

void *CyclicThread::threadFunction( void *context )
{
   return ( (CyclicThread *)context )->cycle();
}
