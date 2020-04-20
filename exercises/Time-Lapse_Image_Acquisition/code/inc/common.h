#ifndef __COMMON_H__
#define __COMMON_H__

#include <pthread.h>
#include <semaphore.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <unordered_map>

enum threads_e
{
   THREAD_SERVER = 0,
   THREAD_CLIENT = 1,
   THREAD_MAX    = THREAD_CLIENT
};

static const char* threadNames[] = {
    "SERVER",
    "CLIENT"};

static threads_e operator++( threads_e thread )
{
   switch ( thread )
   {
      case THREAD_SERVER:
         return THREAD_CLIENT;
      case THREAD_CLIENT:
         return THREAD_SERVER;
      default:
         return THREAD_SERVER;
   }
}

static const uint32_t HRES = 640;
static const uint32_t VRES = 480;

static const uint32_t FRAMES_TO_EXECUTE = 200;

extern bool isTimeToDie;
extern pthread_mutex_t captureLock;
extern pthread_mutex_t windowLock;
extern std::string WindowSize;

extern CvCapture* capture;

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
