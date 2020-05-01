#include <Sequencer.h>
#include <common.h>
#include <configuration.h>
#include <fcntl.h>
#include <logging.h>
#include <syslog.h>
#include <thread.h>
#include <thread_utils.h>

#define USEC_PER_MSEC ( 1000 )
#define SEC_TO_MSEC ( 1000 )
#define NSEC_PER_SEC ( 1000000000 )
#define NSEC_PER_USEC ( 1000000 )

#define EXTRA_CYCLES ( 5 )

Sequencer::Sequencer( uint8_t frequency ) :
    FrameBase( sequencerThreadConfig ),
    captureFrequency( frequency )
{
   uint32_t extraCycles = EXTRA_CYCLES;
   if ( captureFrequency > 1 )
   {
      extraCycles = 30;
   }

   requiredIterations = ( ( FRAMES_TO_EXECUTE + extraCycles ) * SEQUENCER_FREQUENCY ) / captureFrequency;
   executionTimes     = new double[ requiredIterations ]{};
   if ( executionTimes == NULL )
   {
      logging::ERROR( "Mem allocation failed for executionTimes for SEQ", true );
   }

   startTimes = new double[ requiredIterations ]{};
   if ( startTimes == NULL )
   {
      logging::ERROR( "Mem allocation failed for startTimes for SEQ", true );
   }

   endTimes = new double[ requiredIterations ]{};
   if ( endTimes == NULL )
   {
      logging::ERROR( "Mem allocation failed for endTimes for SEQ", true );
   }

   thread = new CyclicThread( sequencerThreadConfig, Sequencer::execute, this, true );
   if ( NULL == thread )
   {
      logging::ERROR( "Could not allocate memory for SEQ Thread", true );
      exit( EXIT_FAILURE );
   }
   alive = true;
}

Sequencer::~Sequencer()
{
   logging::INFO( "Sequencer::~Sequencer() entered", true );
   logging::INFO( "Sequencer::~Sequencer() exiting", true );
}

void Sequencer::sequenceServices()
{
   struct timespec delay_time = {0, 50000000};  // delay for 50 msec, 20Hz
   struct timespec remaining_time;
   double residual;
   int rc, delay_cnt = 0;

   static uint8_t divisor = SEQUENCER_FREQUENCY / captureFrequency;

   do
   {
      delay_cnt = 0;
      residual  = 0.0;

      do
      {
         rc = nanosleep( &delay_time, &remaining_time );

         if ( rc == EINTR )
         {
            residual = remaining_time.tv_sec + ( (double)remaining_time.tv_nsec / (double)NSEC_PER_SEC );

            if ( residual > 0.0 )
               printf( "residual=%lf, sec=%d, nsec=%d\n", residual, (int)remaining_time.tv_sec, (int)remaining_time.tv_nsec );

            delay_cnt++;
         }
         else if ( rc < 0 )
         {
            perror( "Sequencer nanosleep" );
            exit( -1 );
         }

      } while ( ( residual > 0.0 ) && ( delay_cnt < 100 ) );

      // Calculate Start time
      clock_gettime( CLOCK_REALTIME, &start );

      // Store start time in seconds
      startTimes[ count ] = ( (double)start.tv_sec + (double)( ( start.tv_nsec ) / (double)1000000000 ) );

      syslog( LOG_INFO, "SEQ Count: %llu   Sequencer start Time: %lf seconds\n", count, startTimes[ count ] );

      if ( delay_cnt > 1 )
         printf( "Sequencer looping delay %d\n", delay_cnt );

      // Release each service at a sub-rate of the generic sequencer rate
      // Servcie_1 = RT_MAX-1	@ CAPTURE_FREQUENCY (1Hz or 10Hz)
      if ( not abortS1 and ( count % divisor ) == 0 )
      {
         syslog( LOG_INFO, "S1 Release at %llu   Time: %lf seconds\n", count, startTimes[ count ] );
         sem_post( semS1 );
      }

      // Servcie_2 = RT_MAX-1	@ CAPTURE_FREQUENCY (1Hz or 10Hz)
      if ( not abortS2 and ( count % divisor ) == 0 )
      {
         syslog( LOG_INFO, "S2 Release at %llu   Time: %lf seconds\n", count, startTimes[ count ] );
         sem_post( semS2 );
      }

      // Servcie_3 = RT_MAX-1	@ CAPTURE_FREQUENCY (1Hz or 10Hz)
      if ( not abortS3 and ( count % divisor ) == 0 )
      {
         syslog( LOG_INFO, "S3 Release at %llu   Time: %lf seconds\n", count, startTimes[ count ] );
         sem_post( semS3 );
      }

      // Servcie_4 = RT_MIN	@ CAPTURE_FREQUENCY (0.1Hz or 1Hz)
      if ( not abortS4 and ( count % ( divisor * 10 ) ) == 0 )
      {
         syslog( LOG_INFO, "S4 Release at %llu   Time: %lf seconds\n", count, startTimes[ count ] );
         sem_post( semS4 );
      }

      clock_gettime( CLOCK_REALTIME, &end );
      endTimes[ count ] = ( (double)end.tv_sec + (double)( ( end.tv_nsec ) / (double)1000000000 ) );

      executionTimes[ count ] = delta_t( &end, &start );

      syslog( LOG_INFO, "SEQ Count: %llu   Sequencer end Time: %lf seconds\n", count, endTimes[ count ] );

      count++;  //Increment the sequencer count
   } while ( count < requiredIterations );

   abortS1 = true;
   abortS2 = true;
   abortS3 = true;
   abortS4 = true;
   sem_post( semS1 );
   sem_post( semS2 );
   sem_post( semS3 );
   sem_post( semS4 );
   pthread_exit( (void*)0 );
}

void* Sequencer::execute( void* context )
{
   ( (Sequencer*)context )->sequenceServices();
   return NULL;
}