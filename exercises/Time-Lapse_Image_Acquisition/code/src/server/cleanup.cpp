#include <FrameCollector.h>
#include <FrameSender.h>
#include <cleanup.h>
#include <configuration.h>
#include <logging.h>
#include <thread.h>
#include <thread_utils.h>

Cleanup::Cleanup( FrameCollector* collector, FrameSender* sender ) :
    fc( collector ),
    fs( sender )
{
   thread = new CyclicThread( cleanupThreadConfig, Cleanup::execute, this, true );
   if ( thread == NULL )
   {
      logging::ERROR( "Could not allocate memory for Cleanup Thread", true );
      exit( EXIT_FAILURE );
   }
}

Cleanup::~Cleanup()
{
   if ( thread )
   {
      delete thread;
      thread = NULL;
   }
}

void* Cleanup::execute( void* context )
{
   ( (Cleanup*)context )->cleanupService();
   return NULL;
}

void Cleanup::cleanupService( void )
{
   static uint32_t numberDeleted = 0;
   static std::string ppmName( "test_xxxxxxxx.ppm" );

   if ( abortS4 )
   {
      thread->shutdown();
      return;
   }
   sem_wait( semS4 );

   if ( fs )
   {
      if ( fs->isThreadAlive() and ( fs->getFrameCount() > 2000 ) )
      {
         for ( uint8_t i = 0; i < 200; ++i )
         {
            // start deleting if we have sent more than 2000 files
            sprintf( &ppmName.front(), "test_%08d.ppm", numberDeleted );
            if ( std::remove( ppmName.c_str() ) != 0 )
            {
               logging::WARN( logging::getErrnoString( "std::remove" ) );
            }
            else
            {
               numberDeleted++;
            }
         }
      }
   }
}

bool Cleanup::isThreadAlive()
{
   return thread->isThreadAlive();
}

pthread_t Cleanup::getThreadId()
{
   return thread->getThreadId();
}