#ifndef __CIRCULAR_BUFFER_H__
#define __CIRCULAR_BUFFER_H__

#include <memory>

//! @brief Template Circular Buffer Class
template < class T >
class CircularBuffer
{

//! @brief Private data members
private:
   std::unique_ptr< T[] > buffer;  //!< Actual buffer hold items of type T
   size_t head = 0;  //!< Pointer to top of buffer
   size_t tail = 0;  //!< Pointer to bottom of buffer
   size_t maxSize;  //!< Maximum number of items ring can hold
   T emptyItem;  //!< used to clear buffer
public:
   //! Create a new Circular_Buffer.
   CircularBuffer< T >( size_t maxSize ) :
       buffer( std::unique_ptr< T[] >( new T[ maxSize ] ) ), maxSize( maxSize ){};

   //! @brief Add an item to this circular buffer
   //! @param item
   void enqueue( T item );

   //! Remove an item from this circular buffer and return it.
   //! @return T
   T dequeue();

   //! Return the item at the front of this circular buffer.
   //! @return T
   T front();

   //! Check if buffer is empty
   //! @returns true if empty
   //! @returns false otherwise
   bool isEmpty();

   //! Check if buffer is full
   //! @returns true if full
   //! @returns false otherwise
   bool isFull();

   //! Get the size of the buffer
   //! @returns number of items currently in buffer
   size_t size();
};

#endif  // __CIRCULAR_BUFFER_H__