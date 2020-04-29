#ifndef __SEQUENCER_H__
#define __SEQUENCER_H__

#include <FrameBase.h>
#include <common.h>
#include <semaphore.h>

#include <memory>
#include <string>

class CyclicThread;

class Sequencer : public FrameBase
{
public:
   Sequencer( uint8_t frequency );
   ~Sequencer();

   void sequenceServices();
   static void* execute( void* context );

   const uint32_t SEQUENCER_FREQUENCY = 20;  // 20Hz

private:
   uint8_t captureFrequency;
};


inline Sequencer& getSequencer( uint8_t captureFrequency = 1 )
{
   static std::unique_ptr< Sequencer > singleton( new Sequencer( captureFrequency ) );
   return *singleton;
}

#endif  // __SEQUENCER_H__
