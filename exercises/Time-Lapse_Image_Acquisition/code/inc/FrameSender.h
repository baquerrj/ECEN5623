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

private:
   SocketServer* server;
   SocketClient* client;
   char sendBuffer[ IMAGE_SIZE ];
};

#endif  //__FRAME_SENDER_H__