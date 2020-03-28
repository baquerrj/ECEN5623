#ifndef LOGGING_H
#define LOGGING_H

#include <fcntl.h>
#include <mqueue.h>
#include <string.h>
#include <sys/types.h>

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace logging
{
static const char* LOGGER_QUEUE_NAME = "/logger-queue";

enum class log_level : uint8_t
{
   TRACE = 0,
   DEBUG = 1,
   INFO  = 2,
   WARN  = 3,
   ERROR = 4
};

struct message_s
{
   log_level level;
   char msg[ 64 ];
};

struct enum_hasher
{
   template < typename T >
   std::size_t operator()( T t ) const
   {
      return static_cast< std::size_t >( t );
   }
};

const std::unordered_map< log_level, std::string, enum_hasher > tagMap{
    {log_level::ERROR, " [ERROR] "},
    {log_level::WARN, " [WARN] "},
    {log_level::INFO, " [INFO] "},
    {log_level::DEBUG, " [DEBUG] "},
    {log_level::TRACE, " [TRACE] "}};

//logger base class, not pure virtual so you can use as a null logger if you want
using logging_config_t = std::unordered_map< std::string, std::string >;
class logger
{
public:
   logger();
   virtual ~logger();
   virtual void log( const logging::message_s* message, const bool logToStdOut );
   virtual void log( const std::string& message, const log_level level, const bool logToStdout );
   virtual void log( const std::string& message, const bool logToStdout );
   virtual std::string timestamp( void );
   virtual pthread_t getThreadId( void );
   virtual mqd_t getMsgQueueId( void );

protected:
   std::mutex lock;
   mqd_t queue;
   pthread_t threadId;
   struct timespec interval;
   struct timespec lastTime;
   struct timespec currentTime;
   log_level logLevelCutoff;
   const std::unordered_map< log_level, std::string, enum_hasher > levels;

   std::string fileName;
   std::ofstream file;
};

inline pthread_t logger::getThreadId( void )
{
   return threadId;
}

inline mqd_t logger::getMsgQueueId( void )
{
   return queue;
}

inline logger& getLogger( void )
{
   static std::unique_ptr< logger > singleton( new logger() );
   return *singleton;
}

//configure the singleton (once only)
inline void configure( const logging_config_t& config )
{
   getLogger();
}

inline pthread_t getLoggerThreadId( void )
{
   return getLogger().getThreadId();
}

inline void log( const std::string& message, const log_level level )
{
   getLogger().log( message, level, false );
}

inline void log( const std::string& message )
{
   getLogger().log( message, false );
}

//these standout when reading code
inline void TRACE( const std::string& message, const bool logToStdout = false )
{
   getLogger().log( message, log_level::TRACE, logToStdout );
};
inline void DEBUG( const std::string& message, const bool logToStdout = false )
{
   getLogger().log( message, log_level::DEBUG, logToStdout );
};
inline void INFO( const std::string& message, const bool logToStdout = false )
{
   getLogger().log( message, log_level::INFO, logToStdout );
};
inline void WARN( const std::string& message, const bool logToStdout = false )
{
   getLogger().log( message, log_level::WARN, logToStdout );
};
inline void ERROR( const std::string& message, const bool logToStdout = false )
{
   getLogger().log( message, log_level::ERROR, logToStdout );
};

inline void* cycle( void* args )
{
   message_s message = {};
   unsigned int prio = 0;
   while ( 1 )
   {
      memset( &message, 0, sizeof( message ) );
      int retVal = mq_receive( getLogger().getMsgQueueId(), (char*)&message, sizeof( message ), &prio );
      switch ( message.level )
      {
         case logging::log_level::INFO:
         {
            logging::INFO( message.msg );
         }
      }
   }
   return NULL;
}
}  // namespace logging
#endif  // LOGGING_H