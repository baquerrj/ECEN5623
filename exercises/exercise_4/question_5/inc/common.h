#ifndef COMMON_H
#define COMMON_H

#include <pthread.h>
#include <semaphore.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define NSEC_PER_SEC ( 1000000000 )

//#define SHOW_WINDOWS

static const uint32_t HRES = 640;
static const uint32_t VRES = 480;

static const char* window_name[] = {
    "Edge Detector Transform",
    "Hough Line Transform",
    "Hough Elliptical Transform"};

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

extern pid_t mainThreadId;
extern bool isTimeToDie;
extern threadConfig_s* threadConfigs;
extern sem_t syncThreads[ THREAD_MAX ];
extern pthread_mutex_t captureLock;
extern pthread_mutex_t windowLock;

extern CvCapture* capture;

/**
 * @brief calculates the difference between the start and stop times
 *
 * @param stop
 * @param start
 * @param delta_t
 * @return int
 */
int delta_t( struct timespec* stop, struct timespec* start, struct timespec* delta_t );

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

#endif  // COMMON_H