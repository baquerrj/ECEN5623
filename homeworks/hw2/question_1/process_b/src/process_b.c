#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define SEM_NAME ( "/mySemaphore" )

int main( void )
{
   printf( "This process posts a semaphore to release process a\n" );
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

   // post semaphore, releasing process a
   if ( sem_post( sem ) )
   {
      perror( "ERROR - sem_post() " );
      exit( 1 );
   }
   else
   {
      printf( "Released semaphore\n" );
   }

   return 0;
}