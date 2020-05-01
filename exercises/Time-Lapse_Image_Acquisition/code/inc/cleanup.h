#ifndef __CLEANUP_H__
#define __CLEANUP_H__

class CyclicThread;
class FrameCollector;
class FrameSender;

class Cleanup
{
public:
   Cleanup( FrameCollector* collector, FrameSender* sender );
   ~Cleanup();

   void cleanupService();
   static void* execute( void* context );
   bool isThreadAlive();
   pthread_t getThreadId();
private:
   CyclicThread* thread;
   FrameCollector* fc;
   FrameSender* fs;
};

#endif  // __CLEANUP_H__