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
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

//! @brief Namespace defining logging classes, types, etc.
namespace logging
{

//! @brief Name of message queue threads send messages to
static const char* LOGGER_QUEUE_NAME = "/logger-queue";

//! @brief Logging level
enum class log_level : uint8_t
{
   TRACE = 0,
   DEBUG = 1,
   INFO  = 2,
   WARN  = 3,
   ERROR = 4
};

//! @brief Defines structure for message posted to logger's message queue
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

//! @brief Hash map mapping log_level to string representation
const std::unordered_map< log_level, std::string, enum_hasher > tagMap{
    {log_level::ERROR, " [ERROR] "},
    {log_level::WARN, " [WARN] "},
    {log_level::INFO, " [INFO] "},
    {log_level::DEBUG, " [DEBUG] "},
    {log_level::TRACE, " [TRACE] "}};

using logging_config_t = std::unordered_map< std::string, std::string >;

//! Our logger class
class logger
{
public:
   /*! Default constructor */
   logger();
   /*! Default destructor */
   virtual ~logger();
   /*! @brief Logs POSIX message queue
    *
    * @param message
    */
   virtual void log( const logging::message_s* message );

   /*! @brief Logs messages with logging level check
    *
    * @param message
    * @param level
    * @param logToStdout
    */
   virtual void log( const std::string& message, const log_level level, const bool logToStdout );

   /*! @brief Logs messages - not intended to be called directly by other threads
    *
    * @param message
    * @param logToStdout
    */
   virtual void log( const std::string& message, const bool logToStdout );

   /*! @brief Get logger's pthread_t threadId
    *
    * @param void
    * @returns logger::threadId
    */
   virtual pthread_t getThreadId( void );

   /*! @brief Get logger's mqd_t POSIX message queue id
    *
    * @param void
    * @returns logger::queue
    */
   virtual mqd_t getMsgQueueId( void );
private:
   /*! @brief Calculates timestamp
    *
    * @param void
    * @returns formatted timestamp in a std::string buffer
    */
   virtual std::string timestamp( void );

protected:
   pthread_mutex_t lock;  /*! Mutex protecting log file access */
   mqd_t queue;      /*! POSIX message queue id */
   pthread_t threadId;  /*! Thread identifier */
   struct timespec interval;  /*! timespec used to calculate time interval between log calls */
   struct timespec lastTime;  /*! timespec used to calculate time interval between log calls */
   struct timespec currentTime;  /*! used to timestamp and calculate call intervals */
   log_level logLevelCutoff;
   const std::unordered_map< log_level, std::string, enum_hasher > levels;

   std::string fileName;   /*! Name of log file logger writes to */
   std::ofstream file;     /*! Log file handle */
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

inline void log( const logging::message_s* message )
{
   getLogger().log( message );
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