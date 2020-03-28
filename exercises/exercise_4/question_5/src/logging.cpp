#include "logging.h"
#include "common.h"
#include <string.h>

std::string logging::logger::timestamp( void )
{
   //get the time
   struct timespec time;
   clock_gettime( CLOCK_REALTIME, &time );
   //format the string
   std::string buffer( "xxxxxxxxxxxxxxxxxx" );
   sprintf( &buffer.front(), "%ld.%ld", time.tv_sec, time.tv_nsec );
   return buffer;
}

logging::logger::logger() :
    levels( logging::tagMap ),
    logLevelCutoff( logging::log_level::INFO )
{
   mq_unlink( logging::LOGGER_QUEUE_NAME );
   struct mq_attr attr;
   attr.mq_flags   = 0;
   attr.mq_maxmsg  = 100;
   attr.mq_msgsize = sizeof( message_s );
   attr.mq_curmsgs = 0;

   queue = mq_open( logging::LOGGER_QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr );
   if ( 0 > queue )
   {
      throw std::runtime_error( "Could not open queue for logger\n" );
   }

   fileName = std::string( "capture" ) + std::to_string( mainThreadId ) + ".log";
   lock.lock();
   try
   {
      file.open( fileName, std::ofstream::out | std::ofstream::app );
   }
   catch ( const std::exception& e )
   {
      try
      {
         file.close();
      }
      catch ( ... )
      {
         throw e;
      }
   }
   lock.unlock();

   printf( "Created logfile %s\n", fileName.c_str() );
   pthread_create( &threadId, NULL, logging::cycle, NULL );
}

logging::logger::~logger()
{
   mq_unlink( logging::LOGGER_QUEUE_NAME );
}

void logging::logger::log( const logging::message_s* message, const bool logToStdout )
{
   if ( -1 == mq_send( queue, (const char*)message, sizeof( logging::message_s ), 0 ) )
   {
      throw std::runtime_error( "Could not en-queue message!\n" );
   }
}

void logging::logger::log( const std::string& message, const log_level level, const bool logToStdout )
{
   if ( level < logLevelCutoff )
   {
      return;
   }
   std::string output;
   output.reserve( message.length() + 64 );
   output.append( timestamp() );
   output.append( levels.find( level )->second );
   output.append( message );
   output.push_back( '\n' );
   log( output, logToStdout );
}

void logging::logger::log( const std::string& message, const bool logToStdout )
{
   lock.lock();
   file << message;
   file.flush();
   lock.unlock();
   if ( logToStdout == true )
   {
      std::cout << message;
      std::cout.flush();
   }
}
