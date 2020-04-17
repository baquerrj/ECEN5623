#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <common.h>
#include <mqueue.h>
#include <string.h>
#include <sys/types.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <thread_utils.h>
// forward declarations
class CyclicThread;


extern const ThreadConfigData loggerThreadConfigData;
extern const ProcessParams loggerProcessParams;


//! @brief Namespace defining logging classes, types, etc.
namespace logging
{
//! @brief Name of message queue threads send messages to
static const char* LOGGER_QUEUE_NAME = "/Logger-queue";

//! @brief Logging level
enum class LogLevel : uint8_t
{
   TRACE = 0,
   DEBUG = 1,
   INFO  = 2,
   WARN  = 3,
   ERROR = 4
};

//! @brief Defines structure for message posted to Logger's message queue
struct message_s
{
   LogLevel level;
   threads_e ThreadID;
   bool cal_execution;
   char msg[ 64 ];
};

struct enumHasher
{
   template < typename T >
   std::size_t operator()( T t ) const
   {
      return static_cast< std::size_t >( t );
   }
};

//! @brief Hash map mapping LogLevel to string representation
const std::unordered_map< LogLevel, std::string, enumHasher > tagMap{
    {LogLevel::ERROR, " [ERROR] "},
    {LogLevel::WARN, " [WARN] "},
    {LogLevel::INFO, " [INFO] "},
    {LogLevel::DEBUG, " [DEBUG] "},
    {LogLevel::TRACE, " [TRACE] "}};

struct config_s
{
   LogLevel cutoff;
   std::string file;
};

static bool done;

//! Our Logger class
class Logger
{
public:
   //! Default constructor
   Logger( const logging::config_s& config );
   //! Default destructor
   virtual ~Logger();
   /*! @brief Most basic logging
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
   virtual void log( const std::string& message, const LogLevel level, const bool logToStdout );
   /*!
    * @brief log Messages with execution time check
    *
    * @param message
    * @param level
    * @param ThreadID
    * @param execution_time
    * @param logToStdout
    */

   virtual void log( const std::string& message, const LogLevel level, threads_e ThreadID, bool execution_time, const bool logToStdout );

   /*! @brief Logs messages without logging level check
    *
    * @param message
    * @param logToStdout
    */
   virtual void log( const std::string& message, const bool logToStdout );

   /*! @brief Get Logger's pthread_t threadId
    *
    * @param void
    * @returns Logger::threadId
    */
   virtual pthread_t getThreadId( void );

   /*! @brief Get Logger's mqd_t POSIX message queue id
    *
    * @param void
    * @returns Logger::queue
    */
   virtual mqd_t getMsgQueueId( void );

private:
   /*! @brief Calculates timestamp
    *
    * @param void
    * @returns formatted timestamp in a std::string buffer
    */
   virtual std::string timestamp( void );

   /*! @brief Logger thread entry point
    * Logger cycles waiting for messages to show up in its queue
    * and services them as appropriate
    *
    * @param args
    * @returns void pointer
    */
   static void* cycle( void* args );

protected:
   pthread_mutex_t lock;         //! Mutex protecting log file access
   mqd_t queue;                  //! POSIX message queue id
   pthread_t threadId;           //! Thread identifier
   std::unique_ptr< CyclicThread > thread;
   struct timespec interval;     //! timespec used to calculate time interval between log calls
   struct timespec lastTime;     //! timespec used to calculate time interval between log calls
   struct timespec currentTime;  //! used to timestamp and calculate call intervals
   LogLevel logLevelCutoff;
   const std::unordered_map< LogLevel, std::string, enumHasher > levels;

   std::string fileName;  //! Name of log file Logger writes to
   std::ofstream file;    //! Log file handle
};

inline pthread_t Logger::getThreadId( void )
{
   return threadId;
}

inline mqd_t Logger::getMsgQueueId( void )
{
   return queue;
}

inline Logger& getLogger( const config_s& config = {LogLevel::INFO, "capture.log"} )
{
   static std::unique_ptr< Logger > singleton( new Logger( config ) );
   return *singleton;
}

inline void configure( const config_s& config )
{
   getLogger( config );
}

inline pthread_t getLoggerThreadId( void )
{
   return getLogger().getThreadId();
}

inline mqd_t getLoggerMsgQueueId( void )
{
   return getLogger().getMsgQueueId();
}

inline void log( const logging::message_s* message )
{
   getLogger().log( message );
}

inline void log( const std::string& message, const LogLevel level )
{
   getLogger().log( message, level, false );
}

inline void log( const std::string& message, const bool logToStdout = false )
{
   getLogger().log( message, logToStdout );
}

inline void log( const std::string& message, const LogLevel level, const threads_e ThreadID, const bool execution_time, const bool logToStdout )
{
   getLogger().log( message, level, ThreadID, execution_time, false );
}

//these standout when reading code
inline void TRACE( const std::string& message, threads_e ThreadID, bool execution, const bool logToStdout = false )
{
   getLogger().log( message, LogLevel::TRACE, ThreadID, execution, logToStdout );
};
inline void DEBUG( const std::string& message, const bool logToStdout = false )
{
   getLogger().log( message, LogLevel::DEBUG, logToStdout );
};
inline void INFO( const std::string& message, const bool logToStdout = false )
{
   getLogger().log( message, LogLevel::INFO, logToStdout );
};
inline void WARN( const std::string& message, const bool logToStdout = false )
{
   getLogger().log( message, LogLevel::WARN, logToStdout );
};
inline void ERROR( const std::string& message, const bool logToStdout = false )
{
   getLogger().log( message, LogLevel::ERROR, logToStdout );
};

}  // namespace logging
#endif  // __LOGGING_H__