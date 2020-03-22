#ifndef COMMON_H
#define COMMON_H

#include <pthread.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

static const uint32_t HRES = 640;
static const uint32_t VRES = 480;

static const char* window_name[] = {
    "Edge Detector Transform",
    "Hough Line Transform",
    "Hough Elliptical Transform"};

//pthread_t logThread;
//pthread_attr_t logThreadAttr;
//struct sched_param logThreadParam;

enum threads_e
{
   THREAD_CANNY = 0,
   THREAD_HOUGHL,
   THREAD_HOUGHE,
   THREAD_LOGGER,
   THREAD_MAIN,
   THREAD_MAX = THREAD_MAIN
};

struct threadConfig_s
{
   char* threadName[ 50 ];
   pthread_t thread;
   pthread_attr_t atrributes;
   struct sched_param params;
   bool isAlive;
   bool isActive;
};

extern threadConfig_s* threadConfigs;

#endif // COMMON_H