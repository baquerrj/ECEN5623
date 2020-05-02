#ifndef __V4L2_H__
#define __V4L2_H__
#include <errno.h>
#include <linux/videodev2.h>
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
      uint32_t frameNumber;
      struct timespec timestamp;
   };
   static const uint32_t BUFFER_COUNT = 6;

   //! Public Driver Functions
public:
   //! Creates a new V4l2 object
   V4l2( const std::string& deviceName, const V4l2::ioMethod_e method );

   //! Destructor for V4l2 object
   //! Deallocate memory for buffers, undo memory-mapping and close device
   ~V4l2();

   //! Start capturing frames
   void startCapture();

   //! Stop capturing frames
   void stopCapture();

   //! Read frame from camera
   //! @return pointer to captured frame data
   buffer_s* readFrame();

   //! Process image
   //! @todo remove this, FrameProcessor will process image
   void processImage( const void* p, int size );

   //! Low-level wrapper to ioctl system call
   //! @param fh: file descriptor
   //! @param request: request type
   //! @param arg: ioctl argument
   static int8_t xioctl( int8_t fh, uint32_t request, void* arg );

   //! Returns formatted string containing errno description
   std::string getErrnoString( const std::string s );

protected:
   // open video device
   void openDevice();
   // initialize camera
   void initDevice();
   // close video device
   void closeDevice();
   //! Set up device for memory-mapped IO method
   void initMmap();
   //! Setup device for user pointer IO method
   void initUserPtr( unsigned int buffer_size );

private:
   const char* device;      //!< Device name (/dev/video0)
   int8_t fd;               //!< File descriptor for device
   ioMethod_e ioMethod;     //!< Holds IO method for driver
   buffer_s* buffers;       //!< Used to hold raw frame data
   struct v4l2_format fmt;  //<! Stream data format
};

inline std::string V4l2::getErrnoString( const std::string s )
{
   int errnum = errno;
   std::string buffer( std::string( s ) + " ERRNO[" + std::to_string( errnum ) + "]: " + strerror( errnum ) );
   return buffer;
}

#endif  // __V4L2_H__