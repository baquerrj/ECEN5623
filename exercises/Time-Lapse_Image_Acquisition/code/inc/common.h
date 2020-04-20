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
   THREAD_CANNY  = 0,
   THREAD_HOUGHL = 1,
   THREAD_HOUGHE = 2,
   THREAD_MAIN   = 3,
   THREAD_MAX    = THREAD_MAIN
};

static threads_e operator++( threads_e thread )
{
   switch ( thread )
   {
      case THREAD_CANNY:
         return THREAD_HOUGHL;
      case THREAD_HOUGHL:
         return THREAD_HOUGHE;
      case THREAD_HOUGHE:
         return THREAD_CANNY;
      default:
         return THREAD_CANNY;
   }
}

struct threadConfig_s
{
   char* threadName[ 50 ];
   pthread_t thread;
   pthread_attr_t atrributes;
   struct sched_param params;
   bool isAlive;
   bool isActive;
};

struct TransformAnalysis_s
{
   uint32_t deadline_missed;
   double pos_jitter;
   double neg_jitter;
};

static const uint32_t HRES = 640;
static const uint32_t VRES = 480;

static const uint32_t FRAMES_TO_EXECUTE = 200;

static const char* window_name[] = {
    "Edge Detector Transform",
    "Hough Line Transform",
    "Hough Elliptical Transform"};

static const char* thread_name[] = {
    "CANNY",
    "HOUGH-LINES",
    "HOUGH-ELLIP"};

#ifdef SHOW_WINDOWS
static const std::unordered_map< threads_e, std::unordered_map< std::string, uint32_t > > deadlines{
    {THREAD_CANNY, {{"320x240", 3}, {"640x480", 10}, {"1280x960", 40}}},
    {THREAD_HOUGHL, {{"320x240", 45}, {"640x480", 85}, {"1280x960", 210}}},
    {THREAD_HOUGHE, {{"320x240", 10}, {"640x480", 45}, {"1280x960", 290}}}};
#else
static const std::unordered_map< threads_e, std::unordered_map< std::string, uint32_t > > deadlines{
    {THREAD_CANNY, {{"320x240", 2}, {"640x480", 3}, {"1280x960", 5}}},
    {THREAD_HOUGHL, {{"320x240", 7}, {"640x480", 10}, {"1280x960", 22}}},
    {THREAD_HOUGHE, {{"320x240", 3}, {"640x480", 7}, {"1280x960", 33}}}};

#endif

static inline uint32_t getDeadline( threads_e thread, const std::string& res )
{
   auto deadline = ( ( deadlines.find( thread )->second ).find( res )->second );
   return deadline;
}

extern bool isTimeToDie;
extern threadConfig_s* threadConfigs;
extern TransformAnalysis_s* threadAnalysis;
extern sem_t syncThreads[ THREAD_MAX ];
extern pthread_mutex_t captureLock;
extern pthread_mutex_t windowLock;
extern std::string WindowSize;

extern CvCapture* capture;
extern int lowThreshold;
extern int max_lowThreshold;

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

/**
 * @brief calls sem_post on next waiting semaphore.
 *        checks if the thread we wanted to wake up is active
 *        otherwise, recursively calls semPost on the next thread
 *
 * @param thread
 */
inline static void semPost( const threads_e thread )
{
   if ( threadConfigs[ thread ].isActive and &syncThreads[ thread ] )
   {
      sem_post( &syncThreads[ thread ] );
   }
   else
   {
      semPost( ++thread );
   }
}

/**
 * @brief calls sem_wait on a semaphore.
 *        checks if the thread we wait on is active
 *        otherwise, recursively calls semWait on the next thread
 *
 * @param thread
 */
inline static void semWait( const threads_e thread )
{
   if ( threadConfigs[ thread ].isActive and &syncThreads[ thread ] )
   {
      sem_wait( &syncThreads[ thread ] );
   }
   else
   {
      semWait( ++thread );
   }
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
