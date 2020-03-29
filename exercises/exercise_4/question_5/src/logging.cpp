#include "logging.h"

#include <string.h>

#include "common.h"

std::string logging::Logger::timestamp( void )
{
   //get the time
   clock_gettime( CLOCK_REALTIME, &currentTime );
   delta_t( &currentTime, &lastTime, &interval );
   lastTime = currentTime;
   //format the string
   std::string buffer( "xxxxxxxxxxxxxxxxxx" );
   sprintf( &buffer.front(), "%ld.%ld", currentTime.tv_sec, currentTime.tv_nsec );
   return buffer;
}

logging::Logger::Logger() :
    levels( logging::tagMap ),
    logLevelCutoff( logging::log_level::INFO )
{
   mq_unlink( logging::LOGGER_QUEUE_NAME );
   struct mq_attr attr;
   attr.mq_flags   = 0;
   attr.mq_maxmsg  = 10;
   attr.mq_msgsize = sizeof( message_s );
   attr.mq_curmsgs = 0;

   queue = mq_open( logging::LOGGER_QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr );
   if ( 0 > queue )
   {
      throw std::runtime_error( "Could not open queue for Logger\n" );
   }

   fileName = std::string( "capture" ) + std::to_string( mainThreadId ) + ".log";
   pthread_mutex_lock( &lock );
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
   pthread_mutex_unlock( &lock );

   printf( "Created logfile %s\n", fileName.c_str() );
   pthread_create( &threadId, NULL, logging::cycle, NULL );
}

logging::Logger::~Logger()
{
   mq_unlink( logging::LOGGER_QUEUE_NAME );
}

void logging::Logger::log( const logging::message_s* message )
{
   if ( -1 == mq_send( queue, (const char*)message, sizeof( logging::message_s ), 0 ) )
   {
      throw std::runtime_error( "Could not en-queue message!\n" );
   }
}

void logging::Logger::log( const std::string& message, const log_level level, const bool logToStdout )
{
   if ( level < logLevelCutoff )
   {
      return;
   }
   std::string output;
   output.reserve( message.length() + 64 );
   output.append( timestamp() );
   std::string deltaT = std::string( " DT: " ) +
                        std::to_string( (long)interval.tv_sec ) + "." +
                        std::to_string( interval.tv_nsec );
   output.append( levels.find( level )->second );
   output.append( message );
   output.append( deltaT );
   output.push_back( '\n' );
   log( output, logToStdout );
}

void logging::Logger::log( const std::string& message, const bool logToStdout )
{
   pthread_mutex_lock( &lock );
   file << message;
   file.flush();
   pthread_mutex_unlock( &lock );
   if ( logToStdout == true )
   {
      std::cout << message;
      std::cout.flush();
   }
}
