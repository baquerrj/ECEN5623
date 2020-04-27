#ifndef __SEQUENCER_H__
#define __SEQUENCER_H__

#include <common.h>
#include <semaphore.h>

#include <memory>
#include <string>

class CyclicThread;

class Sequencer
{
public:
   Sequencer( uint8_t frequency );
   ~Sequencer();

   void sequenceServices();
   static void* execute( void* context );

   pthread_t getThreadId( void );
   bool isThreadAlive( void );

   const uint32_t SEQUENCER_FREQUENCY = 20;  // 20Hz

private:
   std::string name;
   uint8_t captureFrequency;
   double wcet;            //Store worst case execution for image capturing
   double aet;             //Store average execution time
   double* executionTime;  //To store execution time for each iteration
   double* startTime;      //To store start time for each iteration
   double* endTime;        //To store end time for each iteration

   bool isAlive;
   pthread_t threadId;
   CyclicThread* thread;
};

inline pthread_t Sequencer::getThreadId()
{
   return threadId;
}

inline bool Sequencer::isThreadAlive()
{
   return isAlive;
}

inline Sequencer& getSequecner( uint8_t captureFrequency = 1 )
{
   static std::unique_ptr< Sequencer > singleton( new Sequencer( captureFrequency ) );
   return *singleton;
}

#endif  // __SEQUENCER_H__
