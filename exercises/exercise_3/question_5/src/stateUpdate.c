#include <math.h>  // power
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>  // malloc, etc.
#include <time.h>    // clock_gettime

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
int iterations = 5;
int updates =0;
int gets = 0;
int started = 0;

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

void printState( state_t *state )
{
   printf( "accelX = %2.3f\taccelY = %2.3f\taccelZ = %2.3f\n", state->accelX, state->accelY, state->accelZ );
   printf( "row = %2.3f\tpitch = %2.3f\tyaw = %2.3f\n", state->roll, state->pitch, state->yaw );
   printf( "time: %ld s, %ld ns\n", state->time.tv_sec, state->time.tv_nsec );
}

void *updateState( void *args )
{
   while ( updates < iterations )
   {
      while( gets != updates );
      pthread_mutex_lock( &navLock );
      printf( "Updating state: %dth iteration\n", updates );
      navState->accelX = pow( x, updates );
      navState->accelY = pow( y, updates );
      navState->accelZ = pow( z, updates );
      navState->roll   = pow( roll, updates );
      navState->pitch  = pow( pich, updates );
      navState->yaw    = pow( yaw, updates );
      clock_gettime( CLOCK_REALTIME, &( navState->time ) );
      printf( "Finished updating state\n" );
      pthread_mutex_unlock( &navLock );
      updates++;
   }

   return NULL;
}

void *getState( void *args )
{
   while ( gets < iterations )
   {
      printf( "Reading state: %dth iteration\n", gets );
      pthread_mutex_lock( &navLock );
      printState( navState );
      pthread_mutex_unlock( &navLock );
      gets++;
   }
   return NULL;
}

int main( void )
{
   pthread_t updateStateThread;
   pthread_t getStateThread;
   pthread_attr_t updateStateThreadAttr;
   pthread_attr_t getStateThreadAttr;
   struct sched_param updateStateThreadSchedParam;
   struct sched_param getStateThreadSchedParam;

   pthread_attr_init( &updateStateThreadAttr );
   pthread_attr_setinheritsched( &updateStateThreadAttr, PTHREAD_EXPLICIT_SCHED );
   pthread_attr_setschedpolicy( &updateStateThreadAttr, SCHED_FIFO );
   updateStateThreadSchedParam.sched_priority = 4;

   pthread_attr_init( &getStateThreadAttr );
   pthread_attr_setinheritsched( &getStateThreadAttr, PTHREAD_EXPLICIT_SCHED );
   pthread_attr_setschedpolicy( &getStateThreadAttr, SCHED_FIFO );
   getStateThreadSchedParam.sched_priority = 4;

   pthread_attr_setschedparam( &updateStateThreadAttr, &updateStateThreadSchedParam );
   pthread_attr_setschedparam( &getStateThreadAttr, &getStateThreadSchedParam );

   pthread_mutex_init( &navLock, NULL );

   navState = (state_t *)malloc( sizeof( state_t ) );
   if ( NULL == navState )
   {
      printf( "Failed to allocated memory for navState\n" );
      exit( EXIT_FAILURE );
   }

   // Create the threads
   pthread_create( &updateStateThread, &updateStateThreadAttr, updateState, NULL );
   pthread_create( &getStateThread, &getStateThreadAttr, getState, NULL );

   printf( "Threads started!\n" );

   pthread_join( updateStateThread, NULL );
   pthread_join( getStateThread, NULL );
   pthread_mutex_destroy( &navLock );
   return 0;
}