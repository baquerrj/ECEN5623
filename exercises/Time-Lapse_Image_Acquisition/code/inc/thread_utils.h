#ifndef _THREAD_UTILS_H_
#define _THREAD_UTILS_H_

#include <pthread.h>
#include <stdlib.h>

class CyclicThread
{
public:
   CyclicThread();
   virtual ~CyclicThread();

public:
   pthread_t threadId;
};

#endif // _THREAD_UTILS_H_