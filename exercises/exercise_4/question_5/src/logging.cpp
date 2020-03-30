
#include "logging.h"

#include "common.h"

std::string logging::Logger::timestamp( void )
{
   //get the time
   clock_gettime( CLOCK_REALTIME, &currentTime );
   //format the string
   std::string buffer( "xxxxxxxxxxxxxxxxxx" );
   sprintf( &buffer.front(), "%ld.%ld", currentTime.tv_sec, currentTime.tv_nsec );
   return buffer;
}

logging::Logger::Logger( const logging::config_s& config ) :
    levels( logging::tagMap ),
    logLevelCutoff( config.cutoff ),
    fileName( config.file )
{
   mq_unlink( logging::LOGGER_QUEUE_NAME );
   struct mq_attr attr;
   attr.mq_flags   = 0;
   attr.mq_maxmsg  = 10;
   attr.mq_msgsize = sizeof( logging::message_s );
   attr.mq_curmsgs = 0;

   queue = mq_open( logging::LOGGER_QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr );
   if ( 0 > queue )
   {
      perror( "ERROR: mq_open: " );
      throw std::runtime_error( "Could not open queue for Logger\n" );
   }

   pthread_mutex_init( &lock, NULL );
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
#ifdef USE_FIFO
   int min_prio = sched_get_priority_min( SCHED_FIFO );

   pthread_attr_t threadAttr;
   struct sched_param schedParam;
   pthread_attr_init( &threadAttr );
   pthread_attr_setinheritsched( &threadAttr, PTHREAD_EXPLICIT_SCHED );
   pthread_attr_setschedpolicy( &threadAttr, SCHED_FIFO );
   schedParam.sched_priority = min_prio;
   pthread_attr_setschedparam( &threadAttr, &schedParam );
   pthread_create( &threadId, &threadAttr, logging::cycle, NULL );
#else
   pthread_create( &threadId, NULL, logging::cycle, NULL );
#endif
}

logging::Logger::~Logger()
{
   mq_unlink( logging::LOGGER_QUEUE_NAME );
   pthread_mutex_destroy( &lock );
}

void logging::Logger::log( const logging::message_s* message )
{
   if ( -1 == mq_send( queue, (const char*)message, sizeof( logging::message_s ), 0 ) )
   {
      int errnum = errno;
      logging::ERROR( std::string( strerror( errnum ) ), true );
      throw std::runtime_error( "Could not en-queue message!\n" );
   }
}

void logging::Logger::log( const std::string& message, const LogLevel level, const bool logToStdout )
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
   // Only calculate tack on DT to message if this is a trace
   if ( level == LogLevel::TRACE )
   {
      double dt = delta_t( &currentTime, &lastTime );
      lastTime  = currentTime;
      std::string deltaT( " DT: " +
                          std::to_string( dt ) + " ms" );
      output.append( deltaT );
   }
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
