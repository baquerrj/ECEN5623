#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>
#include <logging.h>

int main( void )
{
   pid_t mainThreadId = getpid();
   std::string fileName     = "capture" + std::to_string( mainThreadId ) + ".log";
   logging::config_s config = {logging::LogLevel::TRACE, fileName};
   logging::configure( config );

   printf("CLIENT HERE!\n");
   return 0;
}

