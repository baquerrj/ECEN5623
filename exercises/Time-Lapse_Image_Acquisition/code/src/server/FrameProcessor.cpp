#include <FrameProcessor.h>
#include <V4l2.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <unistd.h>
#include <thread.h>
#include <fstream>
#include <logging.h>

extern unsigned int framecnt;
extern unsigned char bigbuffer[ ( 1280 * 960 ) ];

extern struct v4l2_format fmt;  //Format is used by a number of functions, so made as a file global

extern sem_t* semS2;
static const ProcessParams processorParams = {
    cpuMain,  // CPU1
    SCHED_FIFO,
    98,  // highest priority
    0};

static const ThreadConfigData processorThreadConfig = {
    true,
    "PROCESSOR",
    processorParams};

FrameProcessor::FrameProcessor()
{
   if ( 0 > sem_init( &sem, 0, 0 ) )
   {
      perror ("FC sem_init failed");
      exit(EXIT_FAILURE);
   }
   thread = new CyclicThread( processorThreadConfig, FrameProcessor::execute, this, true );
}

FrameProcessor::~FrameProcessor()
{
   logging::INFO( "FrameProcessor::~FrameProcessor() entered", true );
   if ( thread )
   {
      delete thread;
      thread = NULL;
   }
   logging::INFO( "FrameProcessor::~FrameProcessor() exiting", true );
}

int FrameProcessor::readFrame()
{
   sem_wait( semS2 );
   return 1;
}

void* FrameProcessor::execute( void* context )
{
   ( (FrameProcessor*)context )->readFrame();
   return NULL;
}

int FrameProcessor::processImage( const void* p, int size )
{
   int i, newi, newsize = 0;
   struct timespec frame_time;
   int y_temp, y2_temp, u_temp, v_temp;
   unsigned char* pptr = (unsigned char*)p;

   // record when process was called
   clock_gettime( CLOCK_REALTIME, &frame_time );

   framecnt++;
   printf( "frame %d: ", framecnt );

   // This just dumps the frame to a file now, but you could replace with whatever image
   // processing you wish.
   //

   if ( fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV )
   {
      printf( "Dump YUYV converted to RGB size %d\n", size );

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

      dumpImage( bigbuffer, ( ( size * 6 ) / 4 ), framecnt, &frame_time );
   }
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

   logging::DEBUG( ppmHeader, true );
   file << ppmHeader;
   file.write( reinterpret_cast< const char* >( p ), size );
   file.close();
   printf( "Wrote %d bytes\n", size );
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
