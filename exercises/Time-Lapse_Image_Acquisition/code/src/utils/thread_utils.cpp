#include <logging.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <thread_utils.h>

#include <cstring>
#include <iostream>
#include <string>
#include <tuple>  // for std::tie

const uint16_t MAX_THREADNAME_LENGTH = 15;  // without '\0'; limit imposed by pthread

const std::string UNSET_POLICY = "UNSPECIFIED";
const int UNSET_POLICY_ID      = -1;
const int UNSET_PRIORITY       = -999;
const int UNSET_NICE           = -999;
const int NUMBER_OF_POLICIES   = 4;

const std::string POLICY[ NUMBER_OF_POLICIES ]{
    UNSET_POLICY,
    "OTHER",
    "RR",
    "FIFO"};

const int POLICY_ID[ NUMBER_OF_POLICIES ]{
    UNSET_POLICY_ID,
    SCHED_OTHER,
    SCHED_RR,
    SCHED_FIFO};

const ProcessParams DEFAULT_PROCESS_PARAMS = {
    -1,
    UNSET_POLICY_ID,
    UNSET_PRIORITY,
    UNSET_NICE};

const ProcessParams VOID_PROCESS_PARAMS = {0, 0, 0, 0};

int numCpus      = NUM_CPUS;
int cpuMain      = CPU_MAIN;
int cpuSequencer = CPU_SEQUENCER;
int cpuCollector = CPU_COLLECTOR;
int cpuProcessor = CPU_PROCESSOR;
int cpuReceiver  = CPU_RECEIVER;
int cpuSender    = CPU_SENDER;
int cpuLogger    = CPU_LOGGER;

static bool name_is_valid( const std::string name );

bool ThreadConfigData::operator==( const ThreadConfigData &that ) const
{
   return this->isValid == that.isValid and
          this->threadName == that.threadName and
          this->processParams == that.processParams;
}
bool ThreadConfigData::operator!=( const ThreadConfigData &that ) const
{
   return !( operator==( that ) );
}

bool ProcessParams::operator==( const ProcessParams &that ) const
{
   return this->cpuId == that.cpuId and
          this->policy == that.policy and
          this->priority == that.priority and
          this->nice == that.nice;
}
bool ProcessParams::operator!=( const ProcessParams &that ) const
{
   return !( operator==( that ) );
}

////////////////////////////////////////////////////////
std::string get_thread_name( const pthread_t &threadId )
{
   char name[ MAX_THREADNAME_LENGTH + 1 ];
   if ( pthread_getname_np( threadId, name, MAX_THREADNAME_LENGTH + 1 ) )
   {
      strcpy( name, "???" );  //@TODO: Should we do something else?
   }
   return std::string( name );
}

////////////////////////////////////////////////////////
void create_thread( const std::string threadName,
                    pthread_t &threadId,
                    void *( *start_routine )(void *),
                    void *args,
                    const ProcessParams &processParams )
{
   if ( threadName.length() > MAX_THREADNAME_LENGTH )
   {
      throw( std::string( "Thread name exceeds 15 characters" ) + " in thread " + threadName );
   }
   if ( not name_is_valid( threadName ) )
   {
      throw( std::string( "Thread name contains invalid characters" ) + " in thread " + threadName );
   }

   pthread_attr_t threadAttr;
   if ( pthread_attr_init( &threadAttr ) )
   {
      throw( std::string( "pthread_attr_init failure" ) + " in thread " + threadName +
             ".  errno: " + std::to_string( (long long)errno ) );
   }

   configure_thread_attributes( threadName, processParams, threadAttr );

   // The nice value of a thread cannot be configured independently via the
   // pthread_create call, so we'll temporarily change the nice value of this
   // calling thread so that the new thread will inherit that value, then we'll
   // restore this thread's nice value after the new thread is created.

   uint16_t originalNice = getpriority( PRIO_PROCESS, 0 );
   setpriority( PRIO_PROCESS, 0, processParams.nice );

   if ( pthread_create( &threadId, &threadAttr, start_routine, args ) )
   {
      throw( std::string( "pthread_create failure [" ) + strerror( errno ) + "] in thread " + threadName );
   }

   setpriority( PRIO_PROCESS, 0, originalNice );

   if ( pthread_setname_np( threadId, threadName.c_str() ) )
   {
      logging::ERROR( std::string( "pthread_setname_np failure [" ) + strerror( errno ) + "] in thread " + threadName );
   }

   if ( pthread_attr_destroy( &threadAttr ) )
   {
      logging::ERROR( std::string( "pthread_attr_destroy failure [" ) + strerror( errno ) + "] in thread " + threadName );
   }
}

////////////////////////////////////////////////////////
void modify_thread( const pthread_t &threadId,
                    const ProcessParams &processParams )
{
   if ( not threadId )
   {
      throw( std::string( "ERROR: Attempting to modify a non-existent thread" ) );
   }

   std::string threadName = get_thread_name( threadId );

   int cpu = processParams.cpuId;

   set_thread_cpu_affinity( threadId, cpu );

   int policy = 0;
   sched_param params{0};
   pthread_getschedparam( threadId, &policy, &params );

   if ( processParams.policy != UNSET_POLICY_ID )
   {
      policy = processParams.policy;
   }
   if ( processParams.priority != UNSET_PRIORITY and
        ( processParams.policy == SCHED_RR or
          processParams.policy == SCHED_FIFO ) )
   {
      params.sched_priority = processParams.priority;
   }

   if ( pthread_setschedparam( threadId, policy, &params ) )
   {
      logging::ERROR( std::string( "Invalid schedule policy and/or priority" ) + " in thread " + threadName + "." );
   }
}

////////////////////////////////////////////////////////
void configure_thread_attributes( const std::string &threadName,
                                  const ProcessParams &processParams,
                                  pthread_attr_t &threadAttr )
{
   int cpu = processParams.cpuId;

   cpu_set_t *cpuSet = NULL;
   size_t cpuSetSize = 0;

   if ( cpu >= 0 and cpu < NUM_CPUS )  // within valid cpu range for setting affinity
   {
      cpuSet = CPU_ALLOC( NUM_CPUS );  // returns NULL on failure
      if ( !cpuSet )
      {
         throw( std::string( "CPU_ALLOC failure" ) + " in thread " + threadName );
      }
      cpuSetSize = CPU_ALLOC_SIZE( NUM_CPUS );
      CPU_ZERO_S( cpuSetSize, cpuSet );
      CPU_SET_S( cpu, cpuSetSize, cpuSet );

      if ( pthread_attr_setaffinity_np( &threadAttr, cpuSetSize, cpuSet ) )
      {
         throw( std::string( "pthread_attr_setaffinity_np failure" ) + " in thread " + threadName );
      }

      if ( processParams.policy != UNSET_POLICY_ID )
      {
         if ( pthread_attr_setinheritsched( &threadAttr, PTHREAD_EXPLICIT_SCHED ) )
         {
            throw( std::string( "pthread_attr_setinheritsched failure" ) + " in thread " + threadName );
         }
         if ( pthread_attr_setschedpolicy( &threadAttr, processParams.policy ) )
         {
            throw( std::string( "pthread_attr_setschedpolicy failure" ) + " in thread " + threadName +
                   " (unsupported policy; must be 'OTHER', 'FIFO', or 'RR')" );
         }
      }

      if ( processParams.priority != UNSET_PRIORITY and
           ( processParams.policy == SCHED_RR or
             processParams.policy == SCHED_FIFO ) )
      {
         sched_param paramSet;
         paramSet.sched_priority = processParams.priority;
         if ( pthread_attr_setschedparam( &threadAttr, &paramSet ) )
         {
            throw( std::string( "pthread_attr_setschedparam failure" ) + " in thread " + threadName +
                   "(specified priority not valid for schedule policy)" );
         }
      }
   }
   else
   {
      if ( cpu >= NUM_CPUS )
      {
         logging::ERROR( std::string( "Invalid CPU # (affinity not set)" ) + " in thread " + threadName );
      }
      // else cpu < 0, indicating that affinity should not be set.
   }
}

////////////////////////////////////////////////////////
void set_thread_cpu_affinity( const pthread_t &threadId,
                              const int cpu )
{
   std::string threadName = get_thread_name( threadId );

   if ( cpu >= NUM_CPUS )
   {
      logging::ERROR( std::string( "Invalid CPU # (affinity not set)" ) + " in thread " + threadName );
      return;
   }
   if ( cpu < 0 )  // indicates that affinity should not be set
   {
      return;
   }

   cpu_set_t *cpuSet = CPU_ALLOC( NUM_CPUS );
   if ( !cpuSet )
   {
      throw( std::string( "CPU_ALLOC failure" ) + " in thread " + threadName );
   }
   size_t cpuSetSize = CPU_ALLOC_SIZE( NUM_CPUS );
   CPU_ZERO_S( cpuSetSize, cpuSet );
   CPU_SET_S( cpu, cpuSetSize, cpuSet );
   if ( pthread_setaffinity_np( threadId, cpuSetSize, cpuSet ) )
   {
      throw( std::string( "pthread_setaffinity_np failure" ) + " in thread " + threadName );
   }
}

////////////////////////////////////////////////////////
void set_this_thread_cpu_affinity( const int cpu, const std::string threadName )
{
   cpu_set_t *cpuSet = CPU_ALLOC( NUM_CPUS );
   if ( !cpuSet )
   {
      throw( std::string( "CPU_ALLOC failure" ) + " in thread " + threadName );
   }
   size_t cpuSetSize = CPU_ALLOC_SIZE( NUM_CPUS );
   CPU_ZERO_S( cpuSetSize, cpuSet );
   CPU_SET_S( cpu, cpuSetSize, cpuSet );
   if ( sched_setaffinity( 0, cpuSetSize, cpuSet ) )
   {
      throw( std::string( "sched_setaffinity failure" ) + " in thread " + threadName );
   }
}

////////////////////////////////////////////////////////
void join_thread( pthread_t &thread, bool &threadIsAlive )
{
   if ( threadIsAlive )
   {
      pthread_join( thread, NULL );
      threadIsAlive = false;
   }
}

////////////////////////////////////////////////////////
void cancel_and_join_thread( pthread_t &thread, bool &threadIsAlive )
{
   if ( threadIsAlive )
   {
      pthread_cancel( thread );
      // pthread_join( thread, NULL );
      threadIsAlive = false;
   }
}

////////////////////////////////////////////////////////
void kill_and_join_thread( pthread_t &thread, bool &threadIsAlive, int signal )
{
   if ( threadIsAlive )
   {
      pthread_kill( thread, signal );
      pthread_join( thread, NULL );
      threadIsAlive = false;
   }
}

////////////////////////////////////////////////////////
static bool name_is_valid( const std::string name )
{
   for ( uint i = 0; i < name.length(); ++i )
   {
      if ( not( isalnum( name[ i ] ) or '_' == name[ i ] ) )
      {
         return false;
      }
   }
   return true;
}

////////////////////////////////////////////////////////