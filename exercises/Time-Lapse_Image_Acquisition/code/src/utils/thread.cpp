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
   printf( "CyclicThread::CyclicThread() entered\n" );
   threadData    = configData;
   threadIsAlive = false;

   if ( readyForThread )
   {
      initiateThread();
   }
   printf( "CyclicThread::CyclicThread() exiting\n" );
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
   printf( "CyclicThread::initiateThread()\n" );
   threadIsAlive = true;
   create_thread( threadData.threadName,
                  thread,
                  CyclicThread::threadFunction,
                  this,
                  threadData.processParams );
}

void CyclicThread::terminate()
{
   cancel_and_join_thread( thread, threadIsAlive );
}

void *CyclicThread::cycle()
{
   while ( threadIsAlive )
   {
      execute( owner );
   }
   logging::INFO( std::string( "thread shutting down: " + threadData.threadName ) );
   pthread_join( thread, NULL );
   return NULL;
}

void *CyclicThread::threadFunction( void *context )
{
   return ( (CyclicThread *)context )->cycle();
}
