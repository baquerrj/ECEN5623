#include <logging.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <thread.h>
#include <unistd.h>

#include <thread.h>
#include <thread_utils.h>

static const ThreadConfigData threadConfigData = {
    true,
    "threadOne",
    DEFAULT_PROCESS_PARAMS};

void* thread_fn( void* args )
{
   logging::INFO( "THREADONE!" );
}

int main( void )
{
   pid_t mainThreadId       = getpid();
   std::string fileName     = "capture" + std::to_string( mainThreadId ) + ".log";
   logging::config_s config = {logging::LogLevel::TRACE, fileName};
   logging::configure( config );

   CyclicThread* thread = static_cast< CyclicThread* >( new CyclicThread( threadConfigData,
                                                                          thread_fn, NULL, true ) );

   printf( "CLIENT HERE!\n" );

   delete thread;

   return 0;
}
