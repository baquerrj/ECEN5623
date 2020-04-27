#include <FrameSender.h>
#include <SocketServer.h>
#include <logging.h>
#include <syslog.h>
#include <thread.h>
#include <thread_utils.h>

static const ProcessParams senderParams = {
    cpuReceiver,  // CPU1
    SCHED_FIFO,
    98,  // highest priority
    0};

static const ThreadConfigData senderThreadConfig = {
    true,
    "SENDER",
    senderParams};

extern const char* host;
extern sem_t* semS3;
const char* patterns[] = {
    "Hello World",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
    "12345671231243",
    "Another message!",
    "One more!"};

FrameSender::FrameSender() :
    name( senderThreadConfig.threadName ),
    wcet( 0.0 ),
    aet( 0.0 ),
    count( 0 ),
    frameCount( 0 ),
    diff_time( 0.0 ),
    start( {0, 0} ),
    end( {0, 0} )
{
   executionTimes = new double[ FRAMES_TO_EXECUTE * 20 ]{};
   if ( executionTimes == NULL )
   {
      logging::ERROR( "Mem allocation failed for executionTimes for FC" );
   }

   startTimes = new double[ FRAMES_TO_EXECUTE * 20 ]{};
   if ( startTimes == NULL )
   {
      logging::ERROR( "Mem allocation failed for startTimes for FC" );
   }

   endTimes = new double[ FRAMES_TO_EXECUTE * 20 ]{};
   if ( endTimes == NULL )
   {
      logging::ERROR( "Mem allocation failed for endTimes for FC" );
   }

   server = new SocketServer( std::string( host ), sockets::DEFAULTPORT );
   if ( server == NULL )
   {
      logging::ERROR( "Could not allocate memory for server", true );
      exit( EXIT_FAILURE );
   }

   thread = new CyclicThread( senderThreadConfig, FrameSender::execute, this, true );
   if ( thread == NULL )
   {
      logging::ERROR( "Could not allocate memory for Sender Thread", true );
      exit( EXIT_FAILURE );
   }

   client = -1;
   server->listen( 1 );
   while ( 0 > client )
   {
      client = server->accept();
   }
   logging::DEBUG( "Connection established", true );
   isAlive = true;
}

FrameSender::~FrameSender()
{
   logging::INFO( "FrameSender::~FrameSender() entered", true );
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
   if ( server )
   {
      delete server;
      server = NULL;
   }
   if ( thread )
   {
      delete thread;
      thread = NULL;
   }
   logging::INFO( "FrameSender::~FrameSender() exiting", true );
}

void* FrameSender::execute( void* context )
{
   ( (FrameSender*)context )->sendPpm();
   return NULL;
}

void FrameSender::sendPpm()
{
   sem_wait( semS3 );
   clock_gettime( CLOCK_REALTIME, &start );
   startTimes[ count ] = ( (double)start.tv_sec + (double)( ( start.tv_nsec ) / (double)1000000000 ) );  //Store start time in seconds

   static int pattern = 0;
   server->send( client, patterns[ pattern ] );
   pattern = ( pattern + 1 ) % 5;
   clock_gettime( CLOCK_REALTIME, &end );                                                          //Get end time of the service
   endTimes[ count ] = ( (double)end.tv_sec + (double)( ( end.tv_nsec ) / (double)1000000000 ) );  //Store end time in seconds
   syslog( LOG_INFO, "%s Count: %lld\t C Time: %lf ms",
           name.c_str(),
           count,
           executionTimes[ count ] );

   logging::DEBUG( "S3 Count: " + std::to_string( count ) +
                   "\t C Time: " + std::to_string( executionTimes[ count ] ) + " ms" );

   count++;
}