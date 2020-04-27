#ifndef __FRAME_SENDER_H__
#define __FRAME_SENDER_H__

#include <semaphore.h>
#include <string.h>

#include <memory>

class CyclicThread;
class SocketServer;

class FrameSender
{
public:
   FrameSender();
   ~FrameSender();

   void sendPpm();

   static void* execute( void* context );

private:
   std::string name;
   double wcet;
   double aet;
   unsigned long long count;
   uint32_t frameCount;
   double diff_time;       //!< To store execution time for each iteration
   struct timespec start;  //!< To measure start time of the service
   struct timespec end;    //!< To measure end time of the service

   bool isAlive;
   sem_t sem;
   CyclicThread* thread;

   double* executionTimes;  //!< To store execution time for each iteration
   double* startTimes;      //!< To store start time for each iteration
   double* endTimes;        //!< To store end time for each iteration
   SocketServer* server;
   int client;
   char *sendBuffer;
};

#endif  //__FRAME_SENDER_H__