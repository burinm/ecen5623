#include <stdint.h>
#include <pthread.h>
#include <stdio.h>
#include <sched.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h> //getpid
#include <unistd.h> //getpid
#include <string.h> //strncmp
#include <assert.h>
#include <math.h>


#define NUM_THREADS 2
#define THREAD_1 1
#define THREAD_2 2

typedef struct
{
    int threadIdx;
} threadParams_t;


threadParams_t threadParams[NUM_THREADS];
pthread_t threads[NUM_THREADS];
struct sched_param nrt_param;

pthread_mutex_t rsrcA = PTHREAD_MUTEX_INITIALIZER;

void *grabRsrcs(void *threadp)
{
   int rc;
   threadParams_t *threadParams = (threadParams_t *)threadp;
   int threadIdx = threadParams->threadIdx;
   struct timespec timeNow;

   int timeout = 0;


assert(threadIdx == THREAD_1 || threadIdx == THREAD_2);

    clock_gettime(CLOCK_REALTIME, &timeNow);
    printf("THREAD %d start %u.%09u nsec\n", threadIdx,
        (int)timeNow.tv_sec, (int)timeNow.tv_nsec);

    if (threadIdx == THREAD_1) {
        timeout = 10;
    }

    if (threadIdx == THREAD_2) {
        timeout = 5;
    }

    while(1) { //keep trying indefinitely


     clock_gettime(CLOCK_REALTIME, &timeNow);
     timeNow.tv_sec += timeout;

     rc=pthread_mutex_timedlock(&rsrcA, &timeNow);
     if (rc == 0)
     {
         printf("Thread %d GOT %p\n", threadIdx, &rsrcA);
     }
     else if(rc == ETIMEDOUT)
     {
         clock_gettime(CLOCK_REALTIME, &timeNow);
         printf("THREAD %d - No new data available at time %lu.%09u\n",
            threadIdx, timeNow.tv_sec, timeNow.tv_nsec);
     }
     else
     {
         printf("Thread %d ERROR\n", threadIdx);
         pthread_exit(NULL);
     }
    }

     printf("THREAD %d done\n", threadIdx);

   pthread_exit(NULL);
}

int main (int argc, char *argv[])
{

   int rc = 0;

   //Grab resources here for demo - code below will never get them
   pthread_mutex_lock(&rsrcA);

   printf("Creating thread %d\n", THREAD_1);
   threadParams[THREAD_1].threadIdx=THREAD_1;
   rc = pthread_create(&threads[0], NULL, grabRsrcs, (void *)&threadParams[THREAD_1]);
   if (rc) {printf("ERROR; pthread_create() rc is %d\n", rc); perror(NULL); exit(-1);}

   printf("Creating thread %d\n", THREAD_2);
   threadParams[THREAD_2].threadIdx=THREAD_2;
   rc = pthread_create(&threads[1], NULL, grabRsrcs, (void *)&threadParams[THREAD_2]);
   if (rc) {printf("ERROR; pthread_create() rc is %d\n", rc); perror(NULL); exit(-1);}

   if(pthread_join(threads[0], NULL) == 0)
     printf("Thread 1 joined to main\n");
   else
     perror("Thread 1");

   if(pthread_join(threads[1], NULL) == 0)
     printf("Thread 2 joined to main\n");
   else
     perror("Thread 2");

   if(pthread_mutex_destroy(&rsrcA) != 0)
     perror("mutex A destroy");

   printf("All done\n");

   exit(0);
}
