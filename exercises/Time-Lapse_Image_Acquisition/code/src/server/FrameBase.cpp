#include <FrameBase.h>
#include <logging.h>
#include <thread.h>
#include <thread_utils.h>

#include <fstream>
#include <iomanip>
#include <limits>

FrameBase::FrameBase( const ThreadConfigData config ) :
    name( config.threadName ),
    wcet( 0.0 ),
    aet( 0.0 ),
    deadline( 0.0 ),
    count( 0 ),
    frameCount( 0 ),
    start( {0, 0} ),
    end( {0, 0} )
{
}

FrameBase::~FrameBase()
{
}

void FrameBase::setDeadline( double deadlineTime )
{
   deadline = deadlineTime;
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

uint32_t FrameBase::getFrameCount()
{
   return frameCount;
}

void FrameBase::jitterAnalysis()
{
   double totalRuntime = 0;
   std::fstream file( name + "_jitter.csv", std::fstream::out );
   if ( file.is_open() )
   {
      file << std::setprecision( std::numeric_limits< double >::digits10 + 1 );
      file << "Count, Start Time (s), End Time (s), Execution Time (ms), Jitter (ms)" << std::endl;
      for ( uint32_t i = 0; i < count; ++i )
      {
         executionTimes[ i ] = endTimes[ i ] - startTimes[ i ];
         if ( executionTimes[ i ] > wcet )
         {
            wcet = executionTimes[ i ];
         }
         totalRuntime += executionTimes[ i ];
      }
      for ( uint32_t i = 0; i < count; ++i )
      {
         double jitter = 0.0;
         if ( i > 0 )
         {
            jitter = ( ( startTimes[ i - 1 ] + deadline ) - startTimes[ i ] ) * 1000.0;
         }
         // Count,  S,   E,   C  jitter
         file << i << "," << startTimes[ i ] << "," << endTimes[ i ] << "," << executionTimes[ i ] << "," << jitter << std::endl;
      }

      aet = totalRuntime / (double)count;
      printf( "(%s) Worst-Case Execution Time = %lf ms\n", name.c_str(), wcet );
      printf( "(%s) Average Execution Time = %lf ms\n", name.c_str(), aet );
   }

   file.flush();
   file.close();
}