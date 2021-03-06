
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
   logging::done = false;
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
   pthread_create( &threadId, &threadAttr, logging::Logger::cycle, NULL );
#else
   pthread_create( &threadId, NULL, logging::Logger::cycle, NULL );
#endif
}

logging::Logger::~Logger()
{
   // Close the message queue
   mq_unlink( logging::LOGGER_QUEUE_NAME );

   // Destroy file mutex and close the file
   pthread_mutex_destroy( &lock );
   file.close();

   // Stop logging thread by setting logging::done to true
   logging::done = true;
   pthread_join( threadId, NULL );
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

void logging::Logger::log( const std::string& message, const LogLevel level, threads_e ThreadID, bool execution_time, const bool logToStdout )
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
      if ( execution_time )
      {
         double deadline = (double)getDeadline( ThreadID, WindowSize );
         if ( dt > deadline )
         {
            threadAnalysis[ ThreadID ].deadline_missed++;
            threadAnalysis[ ThreadID ].pos_jitter += dt - deadline;
         }
         else
         {
            threadAnalysis[ ThreadID ].neg_jitter += deadline - dt;
         }
      }
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

void* logging::Logger::cycle( void* args )
{
   logging::message_s message = {};
   unsigned int prio          = 0;
   struct timespec timeout    = {0};
   while ( !logging::done )
   {
      clock_gettime( CLOCK_REALTIME, &timeout );
      timeout.tv_sec += 2;
      memset( &message, 0, sizeof( message ) );
      if ( 0 > mq_timedreceive( getLoggerMsgQueueId(), (char*)&message, sizeof( message ), &prio, &timeout ) )
      {
         int errnum = errno;
         if ( ETIMEDOUT != errnum )
         {
            logging::ERROR( std::string( strerror( errnum ) ), true );
         }
      }
      else
      {
         switch ( message.level )
         {
            case logging::LogLevel::TRACE:
            {
               logging::TRACE( message.msg, message.ThreadID, message.cal_execution );
               break;
            }
            case logging::LogLevel::DEBUG:
            {
               logging::DEBUG( message.msg );
               break;
            }
            case logging::LogLevel::INFO:
            {
               logging::INFO( message.msg );
               break;
            }
            case logging::LogLevel::WARN:
            {
               logging::WARN( message.msg );
               break;
            }
            case logging::LogLevel::ERROR:
            {
               logging::ERROR( message.msg );
               break;
            }
            default:
            {
               logging::WARN( "Invalid logging level!", true );
               break;
            }
         }
      }
   }
   return NULL;
}