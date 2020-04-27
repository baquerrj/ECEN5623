#include <FrameProcessor.h>
#include <RingBuffer.h>
#include <V4l2.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <logging.h>
#include <syslog.h>
#include <thread.h>
#include <unistd.h>

#include <fstream>

extern unsigned char bigbuffer[ ( 1280 * 960 ) ];

extern struct v4l2_format fmt;  //Format is used by a number of functions, so made as a file global

extern sem_t* semS2;
extern RingBuffer< V4l2::buffer_s > frameBuffer;

static const ProcessParams processorParams = {
    cpuMain,  // CPU1
    SCHED_FIFO,
    98,  // highest priority
    0};

static const ThreadConfigData processorThreadConfig = {
    true,
    "PROCESSOR",
    processorParams};

FrameProcessor::FrameProcessor() :
    name( processorThreadConfig.threadName ),
    wcet( 0.0 ),
    aet( 0.0 ),
    count( 0 ),
    frameCount( 0 ),
    diff_time( 0.0 ),
    start( {0, 0} ),
    end( {0, 0} )
{
   if ( 0 > sem_init( &sem, 0, 0 ) )
   {
      perror( "FC sem_init failed" );
      exit( EXIT_FAILURE );
   }

   executionTimes = new double[ FRAMES_TO_EXECUTE * 20 ]{};
   if ( executionTimes == NULL )
   {
      logging::ERROR( "Mem allocation failed for executionTimes for FP", true );
   }

   startTimes = new double[ FRAMES_TO_EXECUTE * 20 ]{};
   if ( startTimes == NULL )
   {
      logging::ERROR( "Mem allocation failed for startTimes for FP", true );
   }

   endTimes = new double[ FRAMES_TO_EXECUTE * 20 ]{};
   if ( endTimes == NULL )
   {
      logging::ERROR( "Mem allocation failed for endTimes for FP", true );
   }

   thread = new CyclicThread( processorThreadConfig, FrameProcessor::execute, this, true );
   if ( NULL == thread )
   {
      logging::ERROR( "Could not allocate memory for FP Thread", true );
      exit( EXIT_FAILURE );
   }

   isAlive = true;
}

FrameProcessor::~FrameProcessor()
{
   logging::INFO( "FrameProcessor::~FrameProcessor() entered", true );
   if ( thread )
   {
      delete thread;
      thread = NULL;
   }
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
   logging::INFO( "FrameProcessor::~FrameProcessor() exiting", true );
}

int FrameProcessor::readFrame()
{
   sem_wait( semS2 );
   clock_gettime( CLOCK_REALTIME, &start );
   startTimes[ count ] = ( (double)start.tv_sec + (double)( ( start.tv_nsec ) / (double)1000000000 ) );  //Store start time in seconds

   // syslog( LOG_INFO, "S2 Count: %lld   %s Start Time: %lf seconds",
   //         count,
   //         name.c_str(),
   //         startTimes[ count ] );

   if ( !frameBuffer.isEmpty() )
   {
      V4l2::buffer_s img = frameBuffer.dequeue();
      // processImage( img.start, img.length );
      processImage( &img );
   }
   clock_gettime( CLOCK_REALTIME, &end );                                                          //Get end time of the service
   endTimes[ count ] = ( (double)end.tv_sec + (double)( ( end.tv_nsec ) / (double)1000000000 ) );  //Store end time in seconds

   executionTimes[ count ] = delta_t( &end, &start );

   syslog( LOG_INFO, "%s Count: %lld   C Time: %lf ms",
           name.c_str(),
           count,
           executionTimes[ count ] );

   logging::DEBUG( name + " Count: " + std::to_string( count ) +
                   "   C Time: " + std::to_string( executionTimes[ count ] ) + " ms" );
   count++;  //Increment the count of service S2
   return 1;
}

void* FrameProcessor::execute( void* context )
{
   ( (FrameProcessor*)context )->readFrame();
   return NULL;
}

int FrameProcessor::processImage( const void* p, int size )
{
   int i, newi = 0;
   struct timespec frame_time;
   int y_temp, y2_temp, u_temp, v_temp;
   unsigned char* pptr = (unsigned char*)p;

   // record when process was called
   clock_gettime( CLOCK_REALTIME, &frame_time );

   // framecnt++;
   // This just dumps the frame to a file now, but you could replace with whatever image
   // processing you wish.
   //

   // logging::DEBUG( "FP: Frame Count: " + std::to_string( framecnt ) + "Dump YUYV converted to RGB size " + std::to_string( size ), true );

   // Pixels are YU and YV alternating, so YUYV which is 4 bytes
   // We want RGB, so RGBRGB which is 6 bytes
   //
   for ( i = 0, newi = 0; i < size; i = i + 4, newi = newi + 6 )
   {
      y_temp  = (int)pptr[ i ];
      u_temp  = (int)pptr[ i + 1 ];
      y2_temp = (int)pptr[ i + 2 ];
      v_temp  = (int)pptr[ i + 3 ];
      yuv2rgb( y_temp, u_temp, v_temp, &bigbuffer[ newi ], &bigbuffer[ newi + 1 ], &bigbuffer[ newi + 2 ] );
      yuv2rgb( y2_temp, u_temp, v_temp, &bigbuffer[ newi + 3 ], &bigbuffer[ newi + 4 ], &bigbuffer[ newi + 5 ] );
   }

   dumpImage( bigbuffer, ( ( size * 6 ) / 4 ), count, &frame_time );
   return 1;
}

int FrameProcessor::processImage( V4l2::buffer_s* img )
{
   int i, newi = 0;
   struct timespec frame_time = img->timestamp;
   int y_temp, y2_temp, u_temp, v_temp;
   unsigned char* pptr = (unsigned char*)img->start;
   int size            = img->length;

   for ( i = 0, newi = 0; i < size; i = i + 4, newi = newi + 6 )
   {
      y_temp  = (int)pptr[ i ];
      u_temp  = (int)pptr[ i + 1 ];
      y2_temp = (int)pptr[ i + 2 ];
      v_temp  = (int)pptr[ i + 3 ];
      yuv2rgb( y_temp, u_temp, v_temp, &bigbuffer[ newi ], &bigbuffer[ newi + 1 ], &bigbuffer[ newi + 2 ] );
      yuv2rgb( y2_temp, u_temp, v_temp, &bigbuffer[ newi + 3 ], &bigbuffer[ newi + 4 ], &bigbuffer[ newi + 5 ] );
   }

   dumpImage( bigbuffer, ( ( size * 6 ) / 4 ), img->frameNumber, &frame_time );
   return 1;
}

void FrameProcessor::dumpImage( const void* p, int size, unsigned int tag, struct timespec* time )
{
   std::string ppmName( "test_xxxxxxxx.ppm" );
   sprintf( &ppmName.front(), "test_%08d.ppm", tag );
   std::ofstream file;
   file.open( ppmName, std::ofstream::out | std::ofstream::binary );

   std::string ppmHeader( "P6\n#" +
                          std::to_string( (int)time->tv_sec ) + " sec " +
                          std::to_string( (int)( ( time->tv_nsec ) / 1000000 ) ) + " msec \n" +
                          "640 480\n255\n" );

   // logging::DEBUG( "FP: " + ppmHeader, true );
   file << ppmHeader;
   file.write( reinterpret_cast< const char* >( p ), size );
   file.close();
   // printf( "FP: Wrote %d bytes\n", size );
}

void FrameProcessor::yuv2rgb( int y, int u, int v, unsigned char* r, unsigned char* g, unsigned char* b )
{
   int r1, g1, b1;

   // replaces floating point coefficients
   int c = y - 16, d = u - 128, e = v - 128;

   // Conversion that avoids floating point
   r1 = ( 298 * c + 409 * e + 128 ) >> 8;
   g1 = ( 298 * c - 100 * d - 208 * e + 128 ) >> 8;
   b1 = ( 298 * c + 516 * d + 128 ) >> 8;

   // Computed values may need clipping.
   if ( r1 > 255 )
      r1 = 255;
   if ( g1 > 255 )
      g1 = 255;
   if ( b1 > 255 )
      b1 = 255;

   if ( r1 < 0 )
      r1 = 0;
   if ( g1 < 0 )
      g1 = 0;
   if ( b1 < 0 )
      b1 = 0;

   *r = r1;
   *g = g1;
   *b = b1;
}
