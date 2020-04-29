#ifndef __THREAD_UTILS_H__
#define __THREAD_UTILS_H__

#include <pthread.h>

#include <string>

// Thread CPU affinities. (negative value = no affinity specified)
const int NUM_CPUS = 4;  // number of CPU's on the target machine

const int CPU_MAIN      = 0;
const int CPU_SEQUENCER = CPU_MAIN;
const int CPU_COLLECTOR = 1;
const int CPU_PROCESSOR = 2;
const int CPU_RECEIVER  = 3;
const int CPU_SENDER    = 3;
const int CPU_LOGGER    = 3;

extern int numCpus;
extern int cpuMain;
extern int cpuSequencer;
extern int cpuCollector;
extern int cpuProcessor;
extern int cpuReceiver;
extern int cpuSender;
extern int cpuLogger;

extern const uint16_t MAX_THREADNAME_LENGTH;

struct ProcessParams
{
   int cpuId;
   int policy;
   int priority;
   int nice;  // not currently used

   bool operator==( const ProcessParams &that ) const;
   bool operator!=( const ProcessParams &that ) const;
};

extern const ProcessParams DEFAULT_PROCESS_PARAMS;
extern const ProcessParams VOID_PROCESS_PARAMS;

struct ThreadConfigData
{
   bool isValid;
   std::string threadName;
   ProcessParams processParams;

public:
   ThreadConfigData(){};
   ThreadConfigData( const bool isValid,
                     const std::string &threadName,
                     const ProcessParams &processParams ) :
       isValid( isValid ),
       threadName( threadName ),
       processParams( processParams )
   {
   }
   bool operator==( const ThreadConfigData &that ) const;
   bool operator!=( const ThreadConfigData &that ) const;
};

void set_thread_cpu_affinity( const pthread_t &threadId,
                              const int cpu );

void modify_thread( const pthread_t &threadId,
                    const ProcessParams &processParams );

void configure_thread_attributes( const std::string &threadName,
                                  const ProcessParams &processParams,
                                  pthread_attr_t &threadAttr );

void create_thread( const std::string threadName,
                    pthread_t &threadId,
                    void *( *start_routine )(void *),
                    void *args,
                    const ProcessParams &processParams );

void set_this_thread_cpu_affinity( const int cpu,
                                   const std::string threadName );

void join_thread( pthread_t &thread, bool &threadIsAlive );
void cancel_and_join_thread( pthread_t &thread, bool &threadIsAlive );
void kill_and_join_thread( pthread_t &thread, bool &threadIsAlive, int signal );

#endif  // __THREAD_UTILS_H__
