#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <common.h>
#include <string.h>
#include <sys/types.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <unordered_map>

//! @brief Namespace defining logging classes, types, etc.
namespace logging
{
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
   std::string name;
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

//! Our Logger class
class Logger
{
public:
   //! Default constructor
   Logger( const logging::config_s& config );
   //! Default destructor
   virtual ~Logger();

   /*! @brief Logs messages with logging level check
    *
    * @param message
    * @param level
    * @param logToStdout
    */
   virtual void log( const std::string& message, const LogLevel level, const bool logToStdout );

   /*! @brief Logs messages without logging level check
    *
    * @param message
    * @param logToStdout
    */
   virtual void log( const std::string& message, const bool logToStdout );

   std::string getErrnoString( const std::string& s );

private:
   /*! @brief Calculates timestamp
    *
    * @param void
    * @returns formatted timestamp in a std::string buffer
    */
   virtual std::string timestamp( void );

protected:
   const std::unordered_map< LogLevel, std::string, enumHasher > levels;
   LogLevel logLevelCutoff;
   std::string fileName;  //! Name of log file Logger writes to

   pthread_mutex_t lock;  //! Mutex protecting log file access

   struct timespec currentTime;  //! used to timestamp and calculate call intervals

   std::ofstream file;  //! Log file handle
};

inline Logger& getLogger( const config_s& config = {LogLevel::INFO, "capture.log"} )
{
   static std::unique_ptr< Logger > singleton( new Logger( config ) );
   return *singleton;
}

inline void configure( const config_s& config )
{
   getLogger( config );
}

inline void log( const std::string& message, const LogLevel level )
{
   getLogger().log( message, level, false );
}

inline void log( const std::string& message, const bool logToStdout = false )
{
   getLogger().log( message, logToStdout );
}

//these standout when reading code
inline void TRACE( const std::string& message, const bool logToStdout = false )
{
   getLogger().log( message, LogLevel::TRACE, logToStdout );
}
inline void DEBUG( const std::string& message, const bool logToStdout = false )
{
   getLogger().log( message, LogLevel::DEBUG, logToStdout );
}
inline void INFO( const std::string& message, const bool logToStdout = false )
{
   getLogger().log( message, LogLevel::INFO, logToStdout );
}
inline void WARN( const std::string& message, const bool logToStdout = false )
{
   getLogger().log( message, LogLevel::WARN, logToStdout );
}
inline void ERROR( const std::string& message, const bool logToStdout = false )
{
   getLogger().log( message, LogLevel::ERROR, logToStdout );
}

inline std::string Logger::getErrnoString( const std::string& s )
{
   int errnum = errno;
   std::string buffer( std::string( s ) + " ERRNO[" + std::to_string( errnum ) + "]: " + strerror( errnum ) );
   return buffer;
}

inline std::string getErrnoString( const std::string& s )
{
   return getLogger().getErrnoString( s );
}
}  // namespace logging
#endif  // __LOGGING_H__