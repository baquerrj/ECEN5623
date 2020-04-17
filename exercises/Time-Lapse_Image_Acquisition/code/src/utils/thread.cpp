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
    CyclicThread( configData, NULL, NULL, false )  // not supported until gcc 4.7
    //execute( NULL ),
    //owner( NULL )
{
   //threadData    = configData;
   //threadIsAlive = false;
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
   create_thread( threadData.threadName,
                  thread,
                  CyclicThread::threadFunction,
                  this,
                  threadData.processParams );
   threadIsAlive = true;
}

void CyclicThread::terminate()
{
   cancel_and_join_thread( thread, threadIsAlive );
}

void *CyclicThread::cycle()
{
   while ( true )
   {
      execute( owner );
   }
   logging::INFO( std::string( "thread shutting down: " + threadData.threadName ) );
   pthread_exit( NULL );
   return NULL;
}

void *CyclicThread::threadFunction( void *context )
{
   return ( (CyclicThread *)context )->cycle();
}
