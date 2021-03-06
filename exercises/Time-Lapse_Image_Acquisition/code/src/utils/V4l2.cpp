#include <V4l2.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <logging.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// unsigned int framecnt = 0;
// unsigned char bigbuffer[ ( 1280 * 960 ) ];

#define CLEAR( x ) memset( &( x ), 0, sizeof( x ) )

#ifdef NO_FRAME_PROCESSOR
static void dump_ppm( const void* p, int size, unsigned int tag, struct timespec* time );
static void yuv2rgb( int y, int u, int v, unsigned char* r, unsigned char* g, unsigned char* b );
#endif

V4l2::V4l2( const std::string& deviceName, const V4l2::ioMethod_e method )
{
   device   = deviceName.c_str();
   ioMethod = method;
   //buffers = new buffer_s;
   openDevice();
   initDevice();
   startCapture();
}

V4l2::~V4l2()
{
   switch ( ioMethod )
   {
      case V4l2::IO_METHOD_MMAP:
      {
         for ( uint8_t i = 0; i < V4l2::BUFFER_COUNT; ++i )
         {
            if ( -1 == munmap( buffers[ i ].start, buffers[ i ].length ) )
            {
               logging::ERROR( getErrnoString( "munmap" ) );
            }
         }
         break;
      }
      case V4l2::IO_METHOD_USERPTR:
      {
         for ( uint8_t i = 0; i < V4l2::BUFFER_COUNT; ++i )
         {
            free( buffers[ i ].start );
         }
      }
      default:
         break;
   }

   if ( -1 == close( fd ) )
   {
      logging::ERROR( getErrnoString( "close" ) );
   }

   fd = -1;

   //delete buffers;
}

void V4l2::startCapture()
{
   enum v4l2_buf_type type;

   switch ( ioMethod )
   {
      case V4l2::IO_METHOD_MMAP:
      {
         for ( uint8_t i = 0; i < V4l2::BUFFER_COUNT; ++i )
         {
            printf( "allocated buffer %d\n", i );
            struct v4l2_buffer buf;

            CLEAR( buf );
            buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index  = i;

            if ( -1 == xioctl( fd, VIDIOC_QBUF, &buf ) )
            {
               logging::ERROR( getErrnoString( "VIDIOC_QBUF" ) );
            }
         }
         type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
         if ( -1 == xioctl( fd, VIDIOC_STREAMON, &type ) )
         {
            logging::ERROR( getErrnoString( "VIDIOC_STREAMON" ) );
         }
         break;
      }
      case V4l2::IO_METHOD_USERPTR:
      {
         for ( uint8_t i = 0; i < V4l2::BUFFER_COUNT; ++i )
         {
            struct v4l2_buffer buf;

            CLEAR( buf );
            buf.type      = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory    = V4L2_MEMORY_USERPTR;
            buf.index     = i;
            buf.m.userptr = (unsigned long)buffers[ i ].start;
            buf.length    = buffers[ i ].length;

            if ( -1 == xioctl( fd, VIDIOC_QBUF, &buf ) )
            {
               logging::ERROR( getErrnoString( "VIDIOC_QBUF" ) );
            }
         }
         type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
         if ( -1 == xioctl( fd, VIDIOC_STREAMON, &type ) )
         {
            logging::ERROR( getErrnoString( "VIDIOC_STREAMON" ) );
         }
         break;
      }
   }
   return;
}

void V4l2::stopCapture()
{
   enum v4l2_buf_type type;

   type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   if ( -1 == xioctl( fd, VIDIOC_STREAMOFF, &type ) )
   {
      logging::ERROR( getErrnoString( "VIDIOC_STREAMOFF" ) );
   }
   return;
}

int8_t V4l2::xioctl( int8_t fh, uint32_t request, void* arg )
{
   int8_t r;

   do
   {
      r = ioctl( fh, request, arg );

   } while ( -1 == r && EINTR == errno );

   return r;
}

void V4l2::openDevice()
{
   struct stat st;

   if ( -1 == stat( device, &st ) )
   {
      logging::ERROR( getErrnoString( "Cannot identify " + std::string( device ) ), true );
      exit( EXIT_FAILURE );
   }

   if ( !S_ISCHR( st.st_mode ) )
   {
      logging::ERROR( std::string( device ) + "is no device\n", true );
      exit( EXIT_FAILURE );
   }

   fd = open( device, O_RDWR | O_NONBLOCK, 0 );

   if ( -1 == fd )
   {
      logging::ERROR( "Cannot open " + std::string( device ), true );
      exit( EXIT_FAILURE );
   }
   return;
}

void V4l2::initDevice()
{
   struct v4l2_capability cap;
   struct v4l2_cropcap cropcap;
   struct v4l2_crop crop;
   unsigned int min;

   if ( -1 == xioctl( fd, VIDIOC_QUERYCAP, &cap ) )
   {
      if ( EINVAL == errno )
      {
         logging::ERROR( std::string( device ) + " is no V4L2 device", true );
         exit( EXIT_FAILURE );
      }
      else
      {
         {
            logging::ERROR( getErrnoString( "VIDIOC_QUERYCAP" ) );
         }
      }
   }

   if ( !( cap.capabilities & V4L2_CAP_VIDEO_CAPTURE ) )
   {
      logging::ERROR( std::string( device ) + " is no video capture device", true );
      exit( EXIT_FAILURE );
   }

   if ( !( cap.capabilities & V4L2_CAP_STREAMING ) )
   {
      logging::ERROR( std::string( device ) + " does not support streaming i/o", true );
      exit( EXIT_FAILURE );
   }

   /* Select video input, video standard and tune here. */

   CLEAR( cropcap );
   cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

   if ( 0 == xioctl( fd, VIDIOC_CROPCAP, &cropcap ) )
   {
      crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      crop.c    = cropcap.defrect; /* reset to default */

      if ( -1 == xioctl( fd, VIDIOC_S_CROP, &crop ) )
      {
         switch ( errno )
         {
            case EINVAL:
               /* Cropping not supported. */
               break;
            default:
               /* Errors ignored. */
               break;
         }
      }
   }
   else
   {
      /* Errors ignored. */
   }

   CLEAR( fmt );

   fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

   printf( "FORCING FORMAT\n" );
   fmt.fmt.pix.width  = HRES;
   fmt.fmt.pix.height = VRES;

   // Specify the Pixel Coding Formate here

   // This one work for Logitech C200
   fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;

   fmt.fmt.pix.field = V4L2_FIELD_NONE;

   if ( -1 == xioctl( fd, VIDIOC_S_FMT, &fmt ) )
   {
      logging::ERROR( getErrnoString( "VIDIOC_S_FMT" ) );
   }

   /* Buggy driver paranoia. */
   min = fmt.fmt.pix.width * 2;
   if ( fmt.fmt.pix.bytesperline < min )
      fmt.fmt.pix.bytesperline = min;
   min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
   if ( fmt.fmt.pix.sizeimage < min )
      fmt.fmt.pix.sizeimage = min;

   if ( ioMethod == V4l2::IO_METHOD_MMAP )
   {
      initMmap();
   }
   else if ( ioMethod == V4l2::IO_METHOD_USERPTR )
   {
      initUserPtr( fmt.fmt.pix.sizeimage );
   }

   return;
}

void V4l2::closeDevice()
{
   if ( -1 == close( fd ) )
   {
      logging::ERROR( getErrnoString( "close" ) );
   }

   fd = -1;
   return;
}

void V4l2::initMmap()
{
   struct v4l2_requestbuffers req;

   CLEAR( req );

   req.count  = V4l2::BUFFER_COUNT;
   req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   req.memory = V4L2_MEMORY_MMAP;

   if ( -1 == xioctl( fd, VIDIOC_REQBUFS, &req ) )
   {
      if ( EINVAL == errno )
      {
         logging::ERROR( std::string( device ) + " does not support memory mapping", true );
         exit( EXIT_FAILURE );
      }
      else
      {
         {
            logging::ERROR( getErrnoString( "VIDIOC_REQBUFS" ) );
         }
      }
   }

   if ( req.count < 2 )
   {
      logging::ERROR( "Insufficient buffer memory on %s\n", device );
      exit( EXIT_FAILURE );
   }

   buffers = static_cast< V4l2::buffer_s* >( calloc( req.count, sizeof( *buffers ) ) );

   if ( !buffers )
   {
      logging::ERROR( "Out of memory\n" );
      exit( EXIT_FAILURE );
   }

   for ( uint8_t i = 0; i < req.count; ++i )
   {
      struct v4l2_buffer buf;
      CLEAR( buf );

      buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_MMAP;
      buf.index  = i;

      if ( -1 == xioctl( fd, VIDIOC_QUERYBUF, &buf ) )
      {
         logging::ERROR( getErrnoString( "VIDIOC_QUERYBUF" ) );
      }

      buffers[ i ].length = buf.length;
      buffers[ i ].start =
          mmap( NULL /* start anywhere */,
                buf.length,
                PROT_READ | PROT_WRITE /* required */,
                MAP_SHARED /* recommended */,
                fd, buf.m.offset );

      if ( MAP_FAILED == buffers[ i ].start )
      {
         logging::ERROR( getErrnoString( "mmap" ) );
      }
   }
   return;
}

void V4l2::initUserPtr( unsigned int buffer_size )
{
   struct v4l2_requestbuffers req;

   CLEAR( req );

   req.count  = V4l2::BUFFER_COUNT;
   req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   req.memory = V4L2_MEMORY_USERPTR;

   if ( -1 == xioctl( fd, VIDIOC_REQBUFS, &req ) )
   {
      if ( EINVAL == errno )
      {
         logging::ERROR( std::string( device ) +
                         " does not support user pointer i/o" );
         exit( EXIT_FAILURE );
      }
      else
      {
         logging::ERROR( getErrnoString( "VIDIOC_REQBUFS" ) );
      }
   }

   buffers = static_cast< V4l2::buffer_s* >( calloc( req.count, sizeof( *buffers ) ) );

   if ( !buffers )
   {
      logging::ERROR( "Out of memory" );
      exit( EXIT_FAILURE );
   }

   for ( uint8_t i = 0; i < V4l2::BUFFER_COUNT; ++i )
   {
      buffers[ i ].length = buffer_size;
      buffers[ i ].start  = malloc( buffer_size );

      if ( !buffers[ i ].start )
      {
         logging::ERROR( "Out of memory" );
         exit( EXIT_FAILURE );
      }
   }
}

V4l2::buffer_s* V4l2::readFrame( void )
{
   struct v4l2_buffer buf;
   CLEAR( buf );

   switch ( ioMethod )
   {
      case V4l2::IO_METHOD_MMAP:
      {
         buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
         buf.memory = V4L2_MEMORY_MMAP;
         if ( -1 == xioctl( fd, VIDIOC_DQBUF, &buf ) )
         {
            switch ( errno )
            {
               case EAGAIN:
                  return NULL;

               case EIO:
                  /* Could ignore EIO, but drivers should only set for serious errors, although some set for
                     non-fatal errors too. */
                  return NULL;

               default:
               {
                  logging::ERROR( getErrnoString( "mmap VIDIOC_DQBUF" ) );
               }
            }
         }
         //   assert( buf.index < n_buffers );
         buffers[ buf.index ].length = buf.bytesused;
         // processImage( buffers[ buf.index ].start, buf.bytesused );

         if ( -1 == xioctl( fd, VIDIOC_QBUF, &buf ) )
         {
            logging::ERROR( getErrnoString( "VIDIOC_QBUF" ) );
         }
         return &buffers[ buf.index ];
      }
      case V4l2::IO_METHOD_USERPTR:
      {
         buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
         buf.memory = V4L2_MEMORY_USERPTR;
         if ( -1 == xioctl( fd, VIDIOC_DQBUF, &buf ) )
         {
            switch ( errno )
            {
               case EAGAIN:
                  return NULL;
               case EIO:
                  /* Could ignore EIO, see spec. */
               default:
                  logging::ERROR( getErrnoString( "userptr1 VIDIOC_QBUF" ) );
            }
         }

         int idx = 0;
         for ( uint8_t i = 0; i < V4l2::BUFFER_COUNT; ++i )
         {
            if ( buf.m.userptr == (unsigned long)buffers[ i ].start && buf.length == buffers[ i ].length )
            {
               idx = i;
               break;
            }
         }

         // assert( i < n_buffers );

         // processImage( (void*)buf.m.userptr, buf.bytesused );
         // processImage( buffers[ idx ].start, buffers[ idx ].length );

         if ( -1 == xioctl( fd, VIDIOC_QBUF, &buf ) )
         {
            logging::ERROR( getErrnoString( "userptr2 VIDIOC_QBUF" ) );
         }
         return &buffers[ idx ];
      }
   }
   return NULL;
}

void V4l2::processImage( const void* p, int size )
{
#ifdef NO_FRAME_PROCESSOR
   int i, newi = 0;
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

      dump_ppm( bigbuffer, ( ( size * 6 ) / 4 ), framecnt, &frame_time );
   }
#endif
   return;
}

#ifdef NO_FRAME_PROCESSOR
void dump_ppm( const void* p, int size, unsigned int tag, struct timespec* time )
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

void yuv2rgb( int y, int u, int v, unsigned char* r, unsigned char* g, unsigned char* b )
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
#endif