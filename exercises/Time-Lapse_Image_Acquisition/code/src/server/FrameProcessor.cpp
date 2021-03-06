#include <FrameProcessor.h>
#include <RingBuffer.h>
#include <V4l2.h>
#include <configuration.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <logging.h>
#include <sys/utsname.h>
#include <syslog.h>
#include <thread.h>
#include <unistd.h>

#include <fstream>

unsigned char bigbuffer[ ( 1280 * 960 ) ];

extern pthread_mutex_t ringLock;
extern RingBuffer< V4l2::buffer_s > frameBuffer;

extern utsname hostName;

FrameProcessor::FrameProcessor() :
    FrameBase( processorThreadConfig )
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

   alive = true;
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

void FrameProcessor::readFrame()
{
   if ( abortS2 )
   {
      thread->shutdown();
      return;
   }

   sem_wait( semS2 );
   clock_gettime( CLOCK_REALTIME, &start );
   startTimes[ count ] = ( (double)start.tv_sec + (double)( ( start.tv_nsec ) / (double)1000000000 ) );  //Store start time in seconds

   if ( frameCount < FRAMES_TO_EXECUTE )
   {
      if ( !frameBuffer.isEmpty() )
      {
         pthread_mutex_lock( &ringLock );
         V4l2::buffer_s img = frameBuffer.dequeue();
         pthread_mutex_unlock( &ringLock );
         processImage( &img );
         frameCount++;
      }
      else
      {
         syslog( LOG_WARNING, "%s ring buffer EMPTY in cycle %lld", name.c_str(), count );
      }
   }

   clock_gettime( CLOCK_REALTIME, &end );                                                          //Get end time of the service
   endTimes[ count ] = ( (double)end.tv_sec + (double)( ( end.tv_nsec ) / (double)1000000000 ) );  //Store end time in seconds

   executionTimes[ count ] = delta_t( &end, &start );

   syslog( LOG_INFO, "%s Release Count: %lld Frames Processed: %u",
           name.c_str(),
           count,
           frameCount );

   count++;
   return;
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

   std::string ppmHeader;
   ppmHeader.reserve( 200 );
   ppmHeader.append( "P6\n# " +
                     std::to_string( (int)time->tv_sec ) + " sec " +
                     std::to_string( (int)( ( time->tv_nsec ) / 1000000 ) ) + " msec " +
                     hostName.sysname + " " + hostName.nodename + " " +
                     hostName.release + " " + hostName.version + " " + hostName.machine + "\n" +
                     "640 480\n255\n" );
   ppmHeader.resize( 200 );
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