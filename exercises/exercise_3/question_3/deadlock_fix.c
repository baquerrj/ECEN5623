/*
*  Code adapted from  http://mercury.pr.erau.edu/~siewerts/cec450/code/example-sync/
*  Exercise3 Question3: This file solves the deadlock encountered in deadlock.c
*  by using pthread_mutex_timedlock with a random timeout to lock the second resource.
*  compile: call make with included make file
*  usage: $./deadlock_fix
*/



#include <pthread.h>
#include <stdio.h>
#include <sched.h>
#include <time.h>
#include <stdlib.h>

#define NUM_THREADS 2
#define THREAD_1 1
#define THREAD_2 2



pthread_t threads[NUM_THREADS];
struct sched_param nrt_param;
pthread_mutex_t rsrcA, rsrcB; //two mutexes called resource a and b
volatile int rsrcACnt=0, rsrcBCnt=0, noWait=0;
struct timespec timeout_B;
struct timespec timeout_A;
int threadp;

void *grabRsrcs(void *threadp)
{



   if(threadp == THREAD_1)
   {
     printf("THREAD 1 grabbing resources\n");
     pthread_mutex_lock(&rsrcA);
     rsrcACnt++;
     if(!noWait) {sleep(1);}
     printf("THREAD 1 got A, trying for B\n");

	 clock_gettime(CLOCK_REALTIME,&(timeout_B));
	 timeout_B.tv_sec += (rand()) % 10;
	 int rscB_timeout = pthread_mutex_timedlock(&rsrcB,&timeout_B);
 	 if(rscB_timeout  != 0)
	   {
		   pthread_mutex_unlock(&rsrcA);
		   sleep(1);
		   pthread_mutex_lock(&rsrcA);
		   pthread_mutex_lock(&rsrcB);

	   }
	 else
	   {
		   rsrcBCnt++;
		   printf("THREAD 1 got A and B\n");
		   pthread_mutex_unlock(&rsrcB);
		   pthread_mutex_unlock(&rsrcA);
		   printf("THREAD 1 done\n");
	   }
   }
   else
   {
     printf("THREAD 2 grabbing resources\n");
     pthread_mutex_lock(&rsrcB);
     rsrcBCnt++;
     if(!noWait) {sleep(3);}
     printf("THREAD 2 got B, trying for A\n");

	 clock_gettime(CLOCK_REALTIME,&(timeout_A));
	 timeout_A.tv_sec += (rand()) % 10;
	 int rscA_timeout = pthread_mutex_timedlock(&rsrcA,&timeout_A);
 	 if(rscA_timeout  != 0)
	   {
		   pthread_mutex_unlock(&rsrcA);
		   sleep(3);
		   pthread_mutex_lock(&rsrcA);
		   pthread_mutex_lock(&rsrcB);
		    printf("working.... \n");
	   }
	 else
	   {
		rsrcACnt++;
		printf("THREAD 2 got B and A\n");
		pthread_mutex_unlock(&rsrcA);
		pthread_mutex_unlock(&rsrcB);
		printf("THREAD 2 done\n");
	   }


   }
   pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
   int rc, safe=0;

   rsrcACnt=0, rsrcBCnt=0, noWait=0;

   if(argc < 2)
   {
     printf("Will set up unsafe deadlock scenario\n");
   }
   else if(argc == 2)
   {
     if(strncmp("safe", argv[1], 4) == 0)
       safe=1;
     else if(strncmp("race", argv[1], 4) == 0)
       noWait=1;
     else
       printf("Will set up unsafe deadlock scenario\n");
   }
   else
   {
     printf("Usage: deadlock [safe|race|unsafe]\n");
   }

   // Set default protocol for mutex
   pthread_mutex_init(&rsrcA, NULL);
   pthread_mutex_init(&rsrcB, NULL);

   printf("Creating thread %d\n", THREAD_1);
   rc = pthread_create(&threads[0], NULL, grabRsrcs, (void *)THREAD_1);
   if (rc) {printf("ERROR; pthread_create() rc is %d\n", rc); perror(NULL); exit(-1);}
   printf("Thread 1 spawned\n");

   if(safe) // Make sure Thread 1 finishes with both resources first
   {
     if(pthread_join(threads[0], NULL) == 0)
       printf("Thread 1: %x done\n", (unsigned int)threads[0]);
     else
       perror("Thread 1");
   }

   printf("Creating thread %d\n", THREAD_2);
   rc = pthread_create(&threads[1], NULL, grabRsrcs, (void *)THREAD_2);
   if (rc) {printf("ERROR; pthread_create() rc is %d\n", rc); perror(NULL); exit(-1);}
   printf("Thread 2 spawned\n");

   printf("rsrcACnt=%d, rsrcBCnt=%d\n", rsrcACnt, rsrcBCnt);
   printf("will try to join CS threads unless they deadlock\n");

   if(!safe)
   {
     if(pthread_join(threads[0], NULL) == 0)
       printf("Thread 1: %x done\n", (unsigned int)threads[0]);
     else
       perror("Thread 1");
   }

   if(pthread_join(threads[1], NULL) == 0)
     printf("Thread 2: %x done\n", (unsigned int)threads[1]);
   else
     perror("Thread 2");

   if(pthread_mutex_destroy(&rsrcA) != 0)
     perror("mutex A destroy");

   if(pthread_mutex_destroy(&rsrcB) != 0)
     perror("mutex B destroy");

   printf("All done\n");

   exit(0);
}
