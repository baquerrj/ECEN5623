#ifndef __COMMON_H__
#define __COMMON_H__

#include <pthread.h>
#include <semaphore.h>

#include <algorithm>
#include <unordered_map>

#define SEMS1_NAME "/SEMS1"
#define SEMS2_NAME "/SEMS2"
#define SEMS3_NAME "/SEMS3"

static const uint32_t HRES = 640;
static const uint32_t VRES = 480;

static const uint32_t FRAMES_TO_EXECUTE = 10;

/**
 * @brief calculates the difference between the start and stop times
 *
 * @param stop
 * @param start
 * @return double
 */
inline double delta_t( struct timespec* stop, struct timespec* start )
{
   double current = ( (double)stop->tv_sec * 1000.0 ) +
                    ( (double)( (double)stop->tv_nsec / 1000000.0 ) );
   double last = ( (double)start->tv_sec * 1000.0 ) +
                 ( (double)( (double)start->tv_nsec / 1000000.0 ) );
   return ( current - last );
}

inline char* getCmdOption( char** begin, char** end, const std::string& option )
{
   char** itr = std::find( begin, end, option );
   if ( itr != end && ++itr != end )
   {
      return *itr;
   }
   return 0;
}

inline bool cmdOptionExists( char** begin, char** end, const std::string& option )
{
   return std::find( begin, end, option ) != end;
}

#endif  // _COMMON_H_
