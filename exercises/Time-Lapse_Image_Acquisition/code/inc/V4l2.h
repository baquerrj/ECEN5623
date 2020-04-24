#ifndef __V4L2_H__
#define __V4L2_H__
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <string>

class V4l2
{
public:
   V4l2( const std::string& deviceName );
   ~V4l2();

   void startCapture();
   void stopCapture();

   int readFrame();
   void processImage( const void* p, int size );

   static int xioctl( int fh, int request, void* arg );

   void errno_exit( const char* s );

   std::string getErrnoString( const std::string s );

   struct buffer_s
   {
      void* start;
      size_t length;
   };

   static const uint32_t BUFFER_COUNT = 6;

protected:
   // open video device
   void openDevice();
   // initialize camera
   void initDevice();
   // close video device
   void closeDevice();
   void initRead();
   void initMmap();

private:
   const char* device;
   // file descriptor for device
   int fd;

   // buffer_s buffers[ BUFFER_COUNT ];
   buffer_s* buffers;
};

inline void V4l2::errno_exit( const char* s )
{
   fprintf( stderr, "%s error %d, %s\n", s, errno, strerror( errno ) );
   exit( EXIT_FAILURE );
}

inline std::string V4l2::getErrnoString( const std::string s )
{
   int errnum = errno;
   std::string buffer( std::string( s ) + " ERRNO[" + std::to_string( errnum ) + "]: " + strerror( errnum ) );
   return buffer;
}


#endif  // __V4L2_H__