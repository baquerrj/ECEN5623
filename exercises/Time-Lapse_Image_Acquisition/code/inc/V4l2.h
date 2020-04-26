#ifndef __V4L2_H__
#define __V4L2_H__
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>

class V4l2
{
public:
   enum ioMethod_e
   {
      IO_METHOD_MMAP,
      IO_METHOD_USERPTR
   };

   struct buffer_s
   {
      void* start;
      size_t length;
   };

public:
   V4l2( const std::string& deviceName, const V4l2::ioMethod_e method );
   ~V4l2();

   void startCapture();
   void stopCapture();

   buffer_s* readFrame();
   void processImage( const void* p, int size );

   static int8_t xioctl( int8_t fh, uint32_t request, void* arg );

   std::string getErrnoString( const std::string s );

   static const uint32_t BUFFER_COUNT = 4;

protected:
   // open video device
   void openDevice();
   // initialize camera
   void initDevice();
   // close video device
   void closeDevice();
   void initRead();
   void initMmap();
   void initUserPtr(unsigned int buffer_size);

private:
   const char* device;
   // file descriptor for device
   int8_t fd;

   ioMethod_e ioMethod;
   // buffer_s buffers[ BUFFER_COUNT ];
   buffer_s* buffers;
};

inline std::string V4l2::getErrnoString( const std::string s )
{
   int errnum = errno;
   std::string buffer( std::string( s ) + " ERRNO[" + std::to_string( errnum ) + "]: " + strerror( errnum ) );
   return buffer;
}


#endif  // __V4L2_H__