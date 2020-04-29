#include <FrameBase.h>
#include <thread.h>
#include <thread_utils.h>

FrameBase::FrameBase( const ThreadConfigData config ) :
    name( config.threadName ),
    wcet( 0.0 ),
    aet( 0.0 ),
    count( 0 ),
    frameCount( 0 ),
    diff_time( 0.0 ),
    start( {0, 0} ),
    end( {0, 0} )
{
}

FrameBase::~FrameBase()
{
}

bool FrameBase::isAlive()
{
   return alive;
}

bool FrameBase::isThreadAlive()
{
   return thread->isThreadAlive();
}

pthread_t FrameBase::getThreadId()
{
   return thread->getThreadId();
}

void FrameBase::shutdown()
{
   thread->shutdown();
}

sem_t* FrameBase::getSemaphore()
{
   return &sem;
}