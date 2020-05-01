#include <FrameSender.h>
#include <SocketClient.h>
#include <SocketServer.h>
#include <logging.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <syslog.h>
#include <thread.h>
#include <thread_utils.h>
#include <configuration.h>

#define USEC_PER_MSEC ( 1000 )


extern const char* host;

std::string ppmName( "test_xxxxxxxx.ppm" );

using std::to_string;

FrameSender::FrameSender() :
   FrameBase( senderThreadConfig )
{
   //  name = senderThreadConfig.threadName;
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

   server = new SocketServer( SocketBase::TCP_SOCKET );
   if ( server == NULL )
   {
      logging::ERROR( "Could not allocate memory for server", true );
      exit( EXIT_FAILURE );
   }
   server->setupSocket( std::string( host ), DEFAULT_PORT, 3 );

   thread = new CyclicThread( senderThreadConfig, FrameSender::execute, this, true );
   if ( thread == NULL )
   {
      logging::ERROR( "Could not allocate memory for Sender Thread", true );
      exit( EXIT_FAILURE );
   }

   client = new SocketClient();
   if( client == NULL )
   {
      logging::ERROR( "Could not allocate memory for socket", true );
      exit( EXIT_FAILURE );
   }
   while ( false == server->acceptSocket( *client ) )
   {
      ;
   }
   server->setSendFlags( MSG_DONTWAIT );
   logging::DEBUG( "Connection established", true );
   alive = true;
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
   if ( client )
   {
      delete client;
      client = NULL;
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
   if ( abortS3 )
   {
      thread->shutdown();
      return;
   }
   sem_wait( semS3 );
   clock_gettime( CLOCK_REALTIME, &start );
   startTimes[ count ] = ( (double)start.tv_sec + (double)( ( start.tv_nsec ) / (double)1000000000 ) );  //Store start time in seconds
   struct stat st;

   if ( frameCount < FRAMES_TO_EXECUTE )
   {
      sprintf( &ppmName.front(), "test_%08d.ppm", frameCount );

      if ( -1 != stat( ppmName.c_str(), &st ) )
      {
         std::streampos fsize = 0;
         std::ifstream file;
         file.open( ppmName, std::ifstream::in | std::ifstream::binary );
         if ( file )
         {
            file.seekg( 0, file.end );
            fsize = file.tellg();
            file.seekg( 0, file.beg );
         }

         file.read( sendBuffer, fsize );
         if ( file )
         {
            // logging::DEBUG( ppmName + " read into memory" ;
         }
         else
         {
            logging::WARN( "Could only read " + to_string( file.gcount() ), true );
         }

         // logging::DEBUG( "Sending " + to_string( fsize ) + " bytes", true );
         int rc = client->send( (void*)&sendBuffer, fsize );
         if ( 0 > rc )
         {
            logging::WARN( logging::getErrnoString( "send failed" ), true );
         }

         memset( &sendBuffer[ 0 ], 0, sizeof( sendBuffer ) );

         file.close();
         frameCount++;
      }
   }
   else
   {
      // logging::INFO( "SENDER: " + std::to_string( frameCount ) + " frames sent", true );
      abortS3 = true;  // abort on next iteration
   }


   clock_gettime( CLOCK_REALTIME, &end );                                                          //Get end time of the service
   endTimes[ count ] = ( (double)end.tv_sec + (double)( ( end.tv_nsec ) / (double)1000000000 ) );  //Store end time in seconds

   // executionTimes[ count ] = delta_t( &end, &start );
   syslog( LOG_INFO, "%s Release Count: %lld Frames Sent: %u",
           name.c_str(),
           count,
           frameCount );

   // logging::DEBUG( name + " Count: " + std::to_string( count ) +
   //                "   C Time: " + std::to_string( executionTimes[ count ] ) + " ms" );

   count++;
}