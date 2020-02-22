#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define SEM_NAME ("/mySemaphore")

int main( void )
{
   printf( "This process waits on a semaphore\n" );
   sem_t *sem;

   // Try to open semaphore
   sem = sem_open( SEM_NAME, O_RDWR | O_CREAT | O_EXCL, 0666, 0 );
   if ( EEXIST == errno )
   {
      sem = sem_open( SEM_NAME, O_RDWR | O_EXCL, 0 );
   }
   if ( SEM_FAILED == sem )
   {
      perror( "ERROR - sem_open() " );
      exit( 1 );
   }

   // wait on semaphore
   if ( sem_wait( sem ) )
   {
      perror( "ERROR - sem_wait() " );
      exit( 1 );
   }
   else
   {
      printf( "Grabbed semaphore\n" );
   }

   if ( sem_unlink( SEM_NAME ) )
   {
      perror( "ERROR - sem_unlink() " );
      exit( 1 );
   }

   if ( sem_destroy( sem ) )
   {
      perror( "ERROR - sem_destroy() " );
      exit( 1 );
   }

   return 0;
}