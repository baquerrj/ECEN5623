#ifndef __FRAMECOLLECTOR_H__
#define __FRAMECOLLECTOR_H__

#include <FrameBase.h>

class CyclicThread;
class V4l2;

class FrameCollector : public FrameBase
{
public:
   FrameCollector( int device );
   FrameCollector( const FrameCollector& fc );
   ~FrameCollector();

   void terminate();
   void collectFrame( void );
   static void* execute( void* context );

   uint32_t getFrameCount( void );

private:
   V4l2* capture;
};

inline uint32_t FrameCollector::getFrameCount( void )
{
   return frameCount;
}

inline FrameCollector& getCollector( int device = 0 )
{
   static std::unique_ptr< FrameCollector > singleton( new FrameCollector( device ) );
   return *singleton;
}

#endif  // __FRAMECOLLECTOR_H__