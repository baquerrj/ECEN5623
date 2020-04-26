#ifndef __V4L2_H__
#define __V4L2_H__
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <string>

//! @brief V4L2 Driver Class
class V4l2
{
   //! Public Data Types for this class
public:
   enum ioMethod_e
   {
      IO_METHOD_MMAP,    //!< Memory Map IO Method
      IO_METHOD_USERPTR  //!< User Pointer IO Method
   };

   //! Structure to hold pointer to raw image capture data
   struct buffer_s
   {
      void* start;
      size_t length;
   };
   static const uint32_t BUFFER_COUNT = 4;

   //! Public Driver Functions
public:
   //! Creates a new V4l2 object
   V4l2( const std::string& deviceName, const V4l2::ioMethod_e method );
   //! Default DTOR
   ~V4l2();

   //! Start capturing frames
   void startCapture();

   //! Stop capturing frames
   void stopCapture();

   //! Read frame from camera
   //! @return pointer to captured frame data
   buffer_s* readFrame();

   void processImage( const void* p, int size );

   static int8_t xioctl( int8_t fh, uint32_t request, void* arg );

   std::string getErrnoString( const std::string s );

protected:
   // open video device
   void openDevice();
   // initialize camera
   void initDevice();
   // close video device
   void closeDevice();
   void initRead();
   void initMmap();
   void initUserPtr( unsigned int buffer_size );

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