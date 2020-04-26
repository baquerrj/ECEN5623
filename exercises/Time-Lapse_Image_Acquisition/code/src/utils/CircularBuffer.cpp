#include <CircularBuffer.h>
#include <V4l2.h>

template < class T >
void CircularBuffer< T >::enqueue( T item )
{
   // if buffer is full, throw an error
   if ( isFull() )
      throw std::runtime_error( "buffer is full" );

   // insert item at back of buffer
   buffer[ tail ] = item;

   // increment tail
   tail = ( tail + 1 ) % maxSize;
}
template void CircularBuffer< V4l2::buffer_s >::enqueue( V4l2::buffer_s item );

// Remove an item from this circular buffer and return it.
template < class T >
T CircularBuffer< T >::dequeue()
{
   // if buffer is empty, throw an error
   if ( isEmpty() )
      throw std::runtime_error( "buffer is empty" );

   // get item at head
   T item = buffer[ head ];

   // set item at head to be empty
   buffer[ head ] = emptyItem;

   // move head foward
   head = ( head + 1 ) % maxSize;

   // return item
   return item;
}
template V4l2::buffer_s CircularBuffer< V4l2::buffer_s >::dequeue();

// Return the item at the front of this circular buffer.
template < class T >
T CircularBuffer< T >::front()
{
   return buffer[ head ];
}
template V4l2::buffer_s CircularBuffer< V4l2::buffer_s >::front();

// Return true if this circular buffer is empty, and false otherwise.
template < class T >
bool CircularBuffer<T >::isEmpty()
{
   return head == tail;
}
template bool CircularBuffer< V4l2::buffer_s >::isEmpty();

// Return true if this circular buffer is full, and false otherwise.
template < class T >
bool CircularBuffer< T >::isFull()
{
   return tail == ( head - 1 ) % maxSize;
}
template bool CircularBuffer< V4l2::buffer_s >::isFull();

// Return the size of this circular buffer.
template < class T >
size_t CircularBuffer< T >::size()
{
   if ( tail >= head )
      return tail - head;
   return maxSize - (head - tail);
}
template size_t CircularBuffer< V4l2::buffer_s >::size();
