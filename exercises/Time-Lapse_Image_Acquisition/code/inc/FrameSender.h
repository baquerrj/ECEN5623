#ifndef __FRAME_SENDER_H__
#define __FRAME_SENDER_H__

#include <FrameBase.h>
#include <string.h>
#include <memory>

class CyclicThread;
class SocketServer;
class SocketClient;

#define IMAGE_SIZE ( 921800 )

class FrameSender : public FrameBase
{
public:
   FrameSender();
   ~FrameSender();

   void sendPpm();

   static void* execute( void* context );

   uint32_t getNumberSent();
private:
   // std::string name;
   double wcet;
   double aet;
   unsigned long long count;
   uint32_t frameCount;
   double diff_time;       //!< To store execution time for each iteration
   struct timespec start;  //!< To measure start time of the service
   struct timespec end;    //!< To measure end time of the service

   // bool isAlive;
   // sem_t sem;
   // CyclicThread* thread;

   double* executionTimes;  //!< To store execution time for each iteration
   double* startTimes;      //!< To store start time for each iteration
   double* endTimes;        //!< To store end time for each iteration
   SocketServer* server;
   SocketClient* client;
   char sendBuffer[ IMAGE_SIZE ];
   uint32_t framesSent;
};

inline uint32_t FrameSender::getNumberSent()
{
   return framesSent;
}
#endif  //__FRAME_SENDER_H__