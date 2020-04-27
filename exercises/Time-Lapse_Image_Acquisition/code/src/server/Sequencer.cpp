#include <Sequencer.h>
#include <common.h>
#include <fcntl.h>
#include <logging.h>
#include <syslog.h>
#include <thread.h>
#include <thread_utils.h>

#define USEC_PER_MSEC ( 1000 )
#define SEC_TO_MSEC ( 1000 )
#define NSEC_PER_SEC ( 1000000000 )
#define NSEC_PER_USEC ( 1000000 )

static const ProcessParams sequencerParams = {
    cpuMain,  // CPU1
    SCHED_FIFO,
    99,  // highest priority
    0};

static const ThreadConfigData sequencerThreadConfig = {
    true,
    "SEQUENCER",
    sequencerParams};
bool abortTest;
bool abortS1;
bool abortS2;
bool abortS3;
bool abortS4;

extern sem_t* semS1;
extern sem_t* semS2;

Sequencer::Sequencer( uint8_t frequency ) :
    name( sequencerThreadConfig.threadName ),
    captureFrequency( frequency ),
    wcet( 0.0 ),
    aet( 0.0 )
{
   // name = sequencerThreadConfig.threadName;
   // captureFrequency = captureFrequency;
   executionTimes = new double[ FRAMES_TO_EXECUTE * 20 ]{};
   if ( executionTimes == NULL )
   {
      logging::ERROR( "Mem allocation failed for executionTimes for SEQ", true );
   }

   startTimes = new double[ FRAMES_TO_EXECUTE * 20 ]{};
   if ( startTimes == NULL )
   {
      logging::ERROR( "Mem allocation failed for startTimes for SEQ", true );
   }

   endTimes = new double[ FRAMES_TO_EXECUTE * 20 ]{};
   if ( endTimes == NULL )
   {
      logging::ERROR( "Mem allocation failed for endTimes for SEQ", true );
   }
   // semS1 = *_semS1;
   // semS2 = *_semS2;
   thread = new CyclicThread( sequencerThreadConfig, Sequencer::execute, this, true );
   if ( NULL == thread )
   {
      logging::ERROR( "Could not allocate memory for SEQ Thread", true );
      exit( EXIT_FAILURE );
   }
   threadId = thread->getThreadId();
}

Sequencer::~Sequencer()
{
   logging::INFO( "Sequencer::~Sequencer() entered", true );
   if ( executionTimes )
   {
      delete executionTimes;
      executionTimes = NULL;
   }
   if ( startTimes )
   {
      delete startTimes;
      startTimes = NULL;
   }
   if ( endTimes )
   {
      delete endTimes;
      endTimes = NULL;
   }
   if ( thread )
   {
      delete thread;
      thread = NULL;
   }
   logging::INFO( "Sequencer::~Sequencer() exiting", true );
}

void Sequencer::sequenceServices()
{
   struct timespec sequencer_start_time;  //To store start time of sequencer
   struct timespec sequencer_end_time;    //To store end time of sequencer
   // double start_time;                     //To store start time in seconds
   // double end_time;                       //To store end time in seconds

   struct timespec delay_time = {0, 50000000};  // delay for 50 msec, 20Hz
   struct timespec remaining_time;
   double residual;
   int rc, delay_cnt = 0;
   unsigned long long seqCnt = 0;

   static uint8_t divisor = 20 / captureFrequency;
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

      /* Calculate Start time */
      clock_gettime( CLOCK_REALTIME, &sequencer_start_time );

      /* Store start time in seconds */
      // start_time = ( (double)sequencer_start_time.tv_sec + (double)( ( sequencer_start_time.tv_nsec ) / (double)1000000000 ) );
      startTimes[ seqCnt ] = ( (double)sequencer_start_time.tv_sec + (double)( ( sequencer_start_time.tv_nsec ) / (double)1000000000 ) );

      // startTimes[ seqCnt ] = start_time;

      syslog( LOG_INFO, "SEQ Count: %llu\t Sequencer start Time: %lf seconds\n", seqCnt, startTimes[ seqCnt ] );

      if ( delay_cnt > 1 )
         printf( "Sequencer looping delay %d\n", delay_cnt );

      // Release each service at a sub-rate of the generic sequencer rate
      // Servcie_1 = RT_MAX-1	@ 1 Hz
      if ( ( seqCnt % divisor ) == 0 )
      {
         syslog( LOG_INFO, "S1 Release at %llu\t Time: %lf seconds\n", seqCnt, startTimes[ seqCnt ] );
         sem_post( semS1 );
      }

      // Service_2 = RT_MAX-2	@ 1 Hz
      if ( ( seqCnt % divisor ) == 0 )
      {
         syslog( LOG_INFO, "S2 Release at %llu\t Time: %lf seconds\n", seqCnt, startTimes[ seqCnt ] );
         sem_post( semS2 );
      }
      // Service_3 = RT_MAX-3	@ 1 Hz
      //if((seqCnt % 1) == 0) sem_post(semS3);

      clock_gettime( CLOCK_REALTIME, &sequencer_end_time );
      endTimes[ seqCnt ] = ( (double)sequencer_end_time.tv_sec + (double)( ( sequencer_end_time.tv_nsec ) / (double)1000000000 ) );

      syslog( LOG_INFO, "SEQ Count: %llu\t Sequencer end Time: %lf seconds\n", seqCnt, endTimes[ seqCnt ] );

      seqCnt++;  //Increment the sequencer count

   } while ( !abortTest && ( seqCnt < ( FRAMES_TO_EXECUTE * 20 ) ) );

   sem_post( semS1 );
   sem_post( semS2 );
   //sem_post(&semS3);
   abortS1 = true;
   pthread_exit( (void*)0 );
}

void* Sequencer::execute( void* context )
{
   ( (Sequencer*)context )->sequenceServices();
   return NULL;
}