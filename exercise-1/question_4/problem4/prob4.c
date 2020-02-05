/**
 * @\file	prog4.c
 * @\author	Sorabh Gandhi
 * @\brief	This file contains linux version emulation of LCM invariant scheduling
 * @\date	01/30/2020
 *
 * @\Reference	Certain part of this codepiece and logic is borrowed from VxWorks implementation of
 *		Dr.Sam and the independent study paper written by Nisheeth bhai.  
 *
 * @\Limitation	The output and expected performance of this codepiece is only designed and tested 
 *		on linux platform. Running this code on other platform may result in an undeterministic
 *		behaviour
 *
 * @\Notes	This code is only used for educational purpose
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/param.h>
#include <errno.h>
#include <ctype.h>


#define handle_error_en(en, msg) \
	do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define FIB_LIMIT_FOR_32_BIT (47)
#define TIME_PERIOD_FOR_FIB10 (10000)
#define TIME_PERIOD_FOR_FIB20 (20000)
#define SCHEDULING_POLICY SCHED_FIFO
#define INHERIT_SCHEDULER PTHREAD_EXPLICIT_SCHED

sem_t sem_F10,sem_F20;
pthread_attr_t fib10_sched_attr, fib20_sched_attr, main_sched_attr;
struct sched_param fib10_param, fib20_param, main_param, nrt_param;
pthread_t fib10_thread_id, fib20_thread_id, main_thread_id;

uint32_t abortTest = 0;
uint32_t abortTest_20 = 0, abortTest_10 = 0;
uint32_t seqIterations = FIB_LIMIT_FOR_32_BIT;
uint32_t idx = 0, jdx = 1;
uint32_t fib = 0, fib0 = 0, fib1 = 1;
uint32_t fib10Cnt=0, fib20Cnt=0;
uint32_t max_priority;

double start = 0;


#define FIB_TEST(seqCnt, iterCnt)      \
   for(idx=0; idx < iterCnt; idx++)    \
   {                                   \
      fib = fib0 + fib1;               \
      while(jdx < seqCnt)              \
      {                                \
         fib0 = fib1;                  \
         fib1 = fib;                   \
         fib = fib0 + fib1;            \
         jdx++;                        \
      }                                \
   }                                   \


double get_curr_time(void)
{
	struct timeval tv;
	double ft = 0.0;
	if(gettimeofday(& tv, NULL) != 0) {
		perror("readTOD");
		return 0;
	} else {
		ft = ((double)(((double)tv.tv_sec) + (((double)tv.tv_usec) / 1000000.0)));
	}

	return ft;
}

void *fib10(void *arg)
{
	struct sched_param param;
	int policy;
	double stop = 0; 
	unsigned long mask = 1;
	int idx = 0, jdx = 1;
	int fib = 0, fib0 = 0, fib1 = 1;

	while(!abortTest_10) {

		sem_wait (&sem_F10);
		if (pthread_setaffinity_np( pthread_self(), sizeof(mask), &mask) < 0) {
			perror("pthread_setaffinity_np");
		}

		double start_1 = get_curr_time();
		FIB_TEST(seqIterations, 702000);
		stop = get_curr_time();
		pthread_getschedparam(fib10_thread_id, &policy , &param);

		printf("Fib10 priority = %d and time stamp = %lf msec\n", \
			param.sched_priority, (double)(stop - start) * 1000);
   }
}


void *fib20(void *arg)
{
	struct sched_param param;
	int policy;
	double stop = 0;
	unsigned long mask = 1;
	int idx = 0, jdx = 1;
	int fib = 0, fib0 = 0, fib1 = 1;

	while(!abortTest_20) {

		sem_wait(&sem_F20);
		if (pthread_setaffinity_np( pthread_self(), sizeof(mask), &mask) < 0) {
			perror("pthread_setaffinity_np");
		}

		double start_1 = get_curr_time();
		FIB_TEST(seqIterations, 1404000);
		stop = get_curr_time();
		pthread_getschedparam(fib20_thread_id, &policy , &param);

		printf("Fib20 priority = %d and time stamp = %lf msec\n", \
			param.sched_priority, (double)(stop - start) * 1000);
   }
}


void shutdown(void)
{
	abortTest=1;
}


void *sequencer(void *arg)
{
	printf("Starting Sequencer\n");
	unsigned long mask = 1;

	if (pthread_setaffinity_np( pthread_self(), sizeof(mask), &mask) < 0) {
		perror("pthread_setaffinity_np");
	}

	double stop = 0;
	
	usleep(TIME_PERIOD_FOR_FIB20);
 	sem_post (&sem_F10);
 	usleep(TIME_PERIOD_FOR_FIB20);
 	sem_post (&sem_F10);
 	usleep(TIME_PERIOD_FOR_FIB10);
 	abortTest_20 = 1;
 	sem_post (&sem_F20);
 	usleep(TIME_PERIOD_FOR_FIB10);
 	sem_post (&sem_F10);
 	usleep(TIME_PERIOD_FOR_FIB20);
 	abortTest_10 = 1;
 	sem_post (&sem_F10);
 	usleep(TIME_PERIOD_FOR_FIB20);

 	//shutdown();

 	stop = get_curr_time();
	printf("Test Conducted over %lf msec\n",(double)(stop - start) * 1000);
}

int main (void)
{
	int s, rc, scope = 0;
	//useconds_t TIME_PERIOD_FOR_FIB10 = 10000;
	//useconds_t TIME_PERIOD_FOR_FIB20 = 20000;
	abortTest=0;
	
	sem_init (&sem_F10, 0, 1);
 	sem_init (&sem_F20, 0, 1);

 	pthread_attr_init(&fib10_sched_attr);
 	pthread_attr_init(&fib20_sched_attr);
 	pthread_attr_init(&main_sched_attr);

	pthread_attr_setinheritsched(&fib10_sched_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&fib10_sched_attr, SCHED_FIFO);

	pthread_attr_setinheritsched(&fib20_sched_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&fib20_sched_attr, SCHED_FIFO);

	pthread_attr_setinheritsched(&main_sched_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&main_sched_attr, SCHED_FIFO);

	max_priority = sched_get_priority_max(SCHED_FIFO);
	rc=sched_getparam(getpid(), &nrt_param);
	main_param.sched_priority = max_priority;
	fib10_param.sched_priority = (max_priority - 1);
	fib20_param.sched_priority = (max_priority - 2);

	rc = sched_setscheduler(getpid(), SCHED_FIFO, &main_param);
	if (rc) {
		printf("ERROR; sched_setscheduler rc is %d\n", rc); perror(NULL); 
		exit(-1);
	}


	pthread_attr_setschedparam(&fib10_sched_attr, &fib10_param);
	pthread_attr_setschedparam(&fib20_sched_attr, &fib20_param);
	pthread_attr_setschedparam(&main_sched_attr, &main_param);

	start = get_curr_time();
	rc = pthread_create(&fib10_thread_id, &fib10_sched_attr, fib10, NULL);
	if (rc != 0) {
		handle_error_en(s, "Error Creating fib10 thread\n");
	}

	if (pthread_create(&fib20_thread_id, &fib20_sched_attr, fib20, NULL) != 0) {
		handle_error_en(s, "Error Creating fib20 thread\n");
	}

	if (pthread_create(&main_thread_id, &main_sched_attr, sequencer, NULL) != 0) {
		handle_error_en(s, "Error Creating Sequencer thread\n");
	}


	pthread_join(main_thread_id, NULL);
	pthread_join(fib10_thread_id, NULL);
	pthread_join(fib20_thread_id, NULL);

	pthread_attr_destroy(&fib10_sched_attr);
	pthread_attr_destroy(&fib20_sched_attr);
	pthread_attr_destroy(&main_sched_attr);

	sem_destroy(&sem_F10);
 	sem_destroy(&sem_F20);
 	sched_setscheduler(getpid(), SCHED_OTHER, &nrt_param);

 	printf("test complete\n");

	return 0;
}
