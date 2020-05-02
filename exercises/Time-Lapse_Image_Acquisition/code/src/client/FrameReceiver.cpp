#include <FrameReceiver.h>
#include <SocketClient.h>
#include <common.h>
#include <logging.h>
#include <syslog.h>
#include <thread.h>
#include <thread_utils.h>

static const ProcessParams receiverParams = {
    cpuMain,  // CPU1
    SCHED_FIFO,
    99,  // highest priority
    0};

static const ThreadConfigData receiverThreadConfig = {
    true,
    "RECEIVER",
    receiverParams};

std::string ppmName( "test_xxxxxxxx.ppm" );

bool abortSend = false;

extern const char* host;

FrameReceiver::FrameReceiver() :
    FrameBase( receiverThreadConfig )
{
   executionTimes = new double[ FRAMES_TO_EXECUTE * 20 ]{};
   if ( executionTimes == NULL )
   {
      logging::ERROR( "Mem allocation failed for executionTimes for FR" );
   }

   startTimes = new double[ FRAMES_TO_EXECUTE * 20 ]{};
   if ( startTimes == NULL )
   {
      logging::ERROR( "Mem allocation failed for startTimes for FR" );
   }

   endTimes = new double[ FRAMES_TO_EXECUTE * 20 ]{};
   if ( endTimes == NULL )
   {
      logging::ERROR( "Mem allocation failed for endTimes for FR" );
   }

   receiver = new SocketClient( SocketBase::TCP_SOCKET );
   if ( receiver == NULL )
   {
      logging::ERROR( "Could not allocate memory for receiver", true );
      exit( EXIT_FAILURE );
   }
   receiver->setupSocket( std::string( host ), DEFAULT_PORT );

   thread = new CyclicThread( receiverThreadConfig, FrameReceiver::execute, this, true );
   if ( thread == NULL )
   {
      logging::ERROR( "Could not allocate memory for Sender Thread", true );
      exit( EXIT_FAILURE );
   }

   logging::DEBUG( "Connection established", true );
   alive = true;
}

FrameReceiver::~FrameReceiver()
{
   logging::INFO( "FrameReceiver::~FrameReceiver() entered", true );
   if ( executionTimes )
   {
      delete executionTimes;
      executionTimes = NULL;
   }
   if ( startTimes )
   {
      delete startTimes;
      startTimes = NULL;
   }
   if ( endTimes )
   {
      delete endTimes;
      endTimes = NULL;
   }
   if ( receiver )
   {
      delete receiver;
      receiver = NULL;
   }
   if ( thread )
   {
      delete thread;
      thread = NULL;
   }
   logging::INFO( "FrameReceiver::~FrameReceiver() exiting", true );
}

void* FrameReceiver::execute( void* context )
{
   ( (FrameReceiver*)context )->receive();
   return NULL;
}

void FrameReceiver::receive()
{
   if ( frameCount >= FRAMES_TO_EXECUTE )
   {
      thread->shutdown();
      return;
   }

   static std::string remoteHost = receiver->remoteHost();
   static int32_t remotePort     = receiver->remotePort();
   clock_gettime( CLOCK_REALTIME, &start );
   startTimes[ count ] = ( (double)start.tv_sec + (double)( ( start.tv_nsec ) / (double)1000000000 ) );  //Store start time in seconds
   if ( frameCount < FRAMES_TO_EXECUTE )
   {
      int rc             = 0;
      int bytesProcessed = 0;
      sprintf( &ppmName.front(), "test_%08d.ppm", frameCount );
      FILE* fp = fopen( ppmName.c_str(), "w" );
      do
      {
         rc = receiver->receive( (void*)receiveBuffer, IMAGE_SIZE, &remoteHost, &remotePort );
         bytesProcessed += rc;

         fwrite( receiveBuffer, 1, rc, fp );
      } while ( bytesProcessed < IMAGE_SIZE );
      fclose( fp );
      frameCount++;
   }

   clock_gettime( CLOCK_REALTIME, &end );                                                          //Get end time of the service
   endTimes[ count ] = ( (double)end.tv_sec + (double)( ( end.tv_nsec ) / (double)1000000000 ) );  //Store end time in seconds

   // executionTimes[ count ] = delta_t( &end, &start );
   syslog( LOG_INFO, "%s Release Count: %lld Frames Received: %u",
           name.c_str(),
           count,
           frameCount );

   count++;
   return;
}