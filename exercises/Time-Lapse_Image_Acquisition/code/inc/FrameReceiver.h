#ifndef __FRAME_RECEIVER_H__
#define __FRAME_RECEIVER_H__

#include <string.h>
#include <memory>
#include <FrameBase.h>

#define IMAGE_SIZE ( 921800 )

class CyclicThread;
class SocketClient;

class FrameReceiver : public FrameBase
{
public:
   FrameReceiver();
   ~FrameReceiver();

   void receive();

   static void* execute( void* context );

private:
   SocketClient* receiver;
   char receiveBuffer[ IMAGE_SIZE ];
};
#endif  //__FRAME_RECEIVER_H__