#include <FrameBase.h>
#include <thread.h>

FrameBase::FrameBase()
{}

FrameBase::~FrameBase()
{}

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