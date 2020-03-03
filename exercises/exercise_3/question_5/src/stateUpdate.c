#include <math.h>  // power
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>  // malloc, etc.
#include <time.h>    // clock_gettime
#include <unistd.h>  // for sleep()
#include <signal.h>

#define NSECS_PER_SEC   (1e9)
pthread_t updateStateThread;
pthread_t getStateThread;
int stopAndJoin;

// struct defining navigation state
typedef struct
{
   double accelX;
   double accelY;
   double accelZ;
   double roll;
   double pitch;
   double yaw;
   struct timespec time;
} state_t;
state_t *navState;

// mutex to serialize access to nav state
pthread_mutex_t navLock;

// number of times to go through update/get operations
int updates    = 0;

// initial nav data
double x    = 1.1;
double y    = 2.2;
double z    = 3.3;
double roll = 4.4;
double pich = 5.5;
double yaw  = 6.6;

// function prototypes
void *updateState( void *args );
void *getState( void *args );
void printState( state_t *state );
void signalHandler( int sigNum );

void printState( state_t *state )
{
   printf( "******************************\n" );
   printf( "accelX = %2.3f\taccelY = %2.3f\taccelZ = %2.3f\n", state->accelX, state->accelY, state->accelZ );
   printf( "row = %2.3f\tpitch = %2.3f\tyaw = %2.3f\n", state->roll, state->pitch, state->yaw );
   double timestamp = (double)state->time.tv_sec + (double)(state->time.tv_nsec/(double)NSECS_PER_SEC);
   printf( "time: [%lf] s\n", timestamp );
   printf( "******************************\n" );
}

void *updateState( void *args )
{
   while ( stopAndJoin != 1 )
   {
      //while ( gets != updates );
      pthread_mutex_lock( &navLock );
      printf( "******************************\n" );
      printf( "Updating state: %dth iteration\n", updates );
      navState->accelX = pow( x, updates );
      navState->accelY = pow( y, updates );
      navState->accelZ = pow( z, updates );
      navState->roll   = pow( roll, updates );
      navState->pitch  = pow( pich, updates );
      navState->yaw    = pow( yaw, updates );
      sleep(22);
      clock_gettime( CLOCK_REALTIME, &( navState->time ) );
      printf( "Finished updating state\n" );
      printf( "******************************\n" );
      pthread_mutex_unlock( &navLock );
      updates++;
      usleep(1);
   }

   return NULL;
}

void *getState( void *args )
{
   struct timespec timeout;
   timeout.tv_nsec = 0;

   int retVal = 0;
   while ( stopAndJoin != 1 )
   {
      clock_gettime( CLOCK_REALTIME, &timeout );
      timeout.tv_sec += 10;
      //printf( "Checking for new data\n" );
      retVal = pthread_mutex_timedlock( &navLock, &timeout );
      if ( retVal != 0 )
      {
         clock_gettime( CLOCK_REALTIME, &timeout );
         double timestamp = (double)timeout.tv_sec + (double)(timeout.tv_nsec/(double)NSECS_PER_SEC);
         printf( "No new data at time [%lf] s\n", timestamp );
      }
      else
      {
         printState( navState );
         pthread_mutex_unlock( &navLock );
      }
   usleep(1);
   }
   return NULL;
}

void signalHandler( int sigNum )
{
   if( sigNum == SIGINT )
   {
      stopAndJoin = 1;
      printf( "SIGINT received! Stopping...\n" );
   }
}

int main( void )
{
   stopAndJoin = 0;
   pthread_attr_t updateStateThreadAttr;
   pthread_attr_t getStateThreadAttr;

   struct sched_param updateStateThreadSchedParam;
   struct sched_param getStateThreadSchedParam;

   pthread_attr_init( &updateStateThreadAttr );
   pthread_attr_setinheritsched( &updateStateThreadAttr, PTHREAD_EXPLICIT_SCHED );
   pthread_attr_setschedpolicy( &updateStateThreadAttr, SCHED_FIFO );
   updateStateThreadSchedParam.sched_priority = 1;

   pthread_attr_init( &getStateThreadAttr );
   pthread_attr_setinheritsched( &getStateThreadAttr, PTHREAD_EXPLICIT_SCHED );
   pthread_attr_setschedpolicy( &getStateThreadAttr, SCHED_FIFO );
   getStateThreadSchedParam.sched_priority = 1;

   pthread_attr_setschedparam( &updateStateThreadAttr, &updateStateThreadSchedParam );
   pthread_attr_setschedparam( &getStateThreadAttr, &getStateThreadSchedParam );

   pthread_mutex_init( &navLock, NULL );

   navState = (state_t *)malloc( sizeof( state_t ) );
   if ( NULL == navState )
   {
      printf( "Failed to allocated memory for navState\n" );
      exit( EXIT_FAILURE );
   }

   signal( SIGINT, signalHandler );

   // Create the threads
   pthread_create( &updateStateThread, &updateStateThreadAttr, updateState, NULL );
   pthread_create( &getStateThread, &getStateThreadAttr, getState, NULL );
   printf( "Threads started!\n" );

   pthread_join( updateStateThread, NULL );
   pthread_join( getStateThread, NULL );
   pthread_mutex_destroy( &navLock );
   return 0;
}