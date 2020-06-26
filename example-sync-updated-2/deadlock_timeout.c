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
pthread_mutex_t rsrcB = PTHREAD_MUTEX_INITIALIZER;

/*These do not need to be volatile, mutex is about seqential data access,
   flags are not modified in an ISR - or ever for that matter
*/
int rsrcCnts[NUM_THREADS] = {0};
#define RESOURCE_A_ID   0
#define RESOURCE_B_ID   1

int noWait=0;

uint16_t rng16();

//manipulated with atomic fetch and add - may skip values, but we don't care
volatile uint16_t backoff = 1; //start with 100ms

void *grabRsrcs(void *threadp)
{
   struct timespec timeNow;
   struct timespec rsrc1_timeout;
   struct timespec rsrc2_timeout;
   int rc;
   threadParams_t *threadParams = (threadParams_t *)threadp;
   int threadIdx = threadParams->threadIdx;

   if(threadIdx == THREAD_1) printf("Thread 1 started\n");
   else if(threadIdx == THREAD_2) printf("Thread 2 started\n");
   else printf("Unknown thread started\n");

   clock_gettime(CLOCK_REALTIME, &timeNow);

   //rsrc1_timeout.tv_sec = timeNow.tv_sec + 2;
   //rsrc1_timeout.tv_nsec = timeNow.tv_nsec;
   //rsrc2_timeout.tv_sec = timeNow.tv_sec + 3;
   //rsrc2_timeout.tv_nsec = timeNow.tv_nsec;


assert(threadIdx == THREAD_1 || threadIdx == THREAD_2);

   int got_one = 0;
   int got_two = 0;

   pthread_mutex_t *resource_to_aquire_one;
   pthread_mutex_t *resource_to_aquire_two;
   int resource_one_id = -1;
   int resource_two_id = -1;

   if(threadIdx == THREAD_1) {
        resource_to_aquire_one = &rsrcA;
        resource_to_aquire_two = &rsrcB;
        resource_one_id = RESOURCE_A_ID;
        resource_two_id = RESOURCE_B_ID;
   }

   if(threadIdx == THREAD_2) {
        resource_to_aquire_one = &rsrcB;
        resource_to_aquire_two = &rsrcA;
        resource_one_id = RESOURCE_B_ID;
        resource_two_id = RESOURCE_A_ID;
   }

assert(resource_one_id != -1);
assert(resource_two_id != -1);

    while(1) { //keep trying indefinitely to aquire both resources

     printf("THREAD %d grabbing resource %p @ %d sec and %d nsec\n", threadIdx, resource_to_aquire_one,
                                                            (int)timeNow.tv_sec, (int)timeNow.tv_nsec);

     int delay1 = (int)pow(2, backoff);
     printf("Thread %d backoff.. %dms\n", threadIdx, delay1 * 100);

     clock_gettime(CLOCK_REALTIME, &timeNow);
     rsrc1_timeout.tv_sec = timeNow.tv_sec + delay1;
     rsrc1_timeout.tv_nsec = timeNow.tv_nsec + delay1 * 10000000;
     __sync_fetch_and_add(&backoff, 1);

     if((rc=pthread_mutex_timedlock(resource_to_aquire_one, &rsrc1_timeout)) != 0)
     {
         printf("Thread %d ERROR\n", threadIdx);
         pthread_exit(NULL);
     }
     else if(rc == ETIMEDOUT)
     {
         printf("Thread %d aquiring %p TIMEOUT\n", threadIdx, resource_to_aquire_one);
     }
     else
     {
         printf("Thread %d GOT %p\n", threadIdx, resource_to_aquire_one);
         rsrcCnts[resource_one_id]++;
         printf("resource A (%p) count=%d, resource B (%p) count=%d\n", &rsrcA, rsrcCnts[RESOURCE_A_ID],
                                                                        &rsrcB, rsrcCnts[RESOURCE_B_ID]);
        got_one = 1;
     }

    if (got_one && got_two) {
        break;
    }

     // if unsafe test, immediately try to acquire resource two 
     if(!noWait) usleep(1000000);

     //uint16_t random_number = backoff();
     //printf("backoff.. %dns", random_number);
     int delay2 = (int)pow(2, backoff);
     printf("Thread %d backoff.. %dms\n", threadIdx, delay2 * 100);

     clock_gettime(CLOCK_REALTIME, &timeNow);
     rsrc2_timeout.tv_sec = timeNow.tv_sec;
     rsrc2_timeout.tv_nsec = timeNow.tv_nsec + delay2 * 10000000;
     __sync_fetch_and_add(&backoff, 1);

     printf("THREAD %d got %p, trying for %p @ %d sec and %d nsec\n",  threadIdx, resource_to_aquire_one,
                                                            resource_to_aquire_two,
                                                            (int)timeNow.tv_sec, (int)timeNow.tv_nsec);

     rc=pthread_mutex_timedlock(resource_to_aquire_two, &rsrc2_timeout);
     //rc=pthread_mutex_lock(&rsrcB);
     if(rc == 0)
     {
         clock_gettime(CLOCK_REALTIME, &timeNow);
         printf("Thread %d GOT %p @ %d sec and %d nsec with rc=%d\n", threadIdx, resource_to_aquire_two,
                                                                    (int)timeNow.tv_sec, (int)timeNow.tv_nsec, rc);
         rsrcCnts[resource_two_id]++;
         printf("resource A (%p) count=%d, resource B (%p) count=%d\n", &rsrcA, rsrcCnts[RESOURCE_A_ID],
                                                                        &rsrcB, rsrcCnts[RESOURCE_B_ID]);
        got_two = 1;
     }
     else if(rc == ETIMEDOUT)
     {
         printf("Thread %d TIMEOUT ERROR\n", threadIdx);
         rsrcCnts[resource_one_id]--;
         pthread_mutex_unlock(resource_to_aquire_one);
         continue;
     }
     else
     {
         printf("Thread %d ERROR\n", threadIdx);
         rsrcCnts[resource_one_id]--;
         pthread_mutex_unlock(resource_to_aquire_one);
         pthread_exit(NULL);
     }

     if (got_one && got_two) {
         break;
     }

    }

     printf("THREAD %d got %p and %p\n", threadIdx, resource_to_aquire_one, resource_to_aquire_two);
     rsrcCnts[resource_two_id]--;
     pthread_mutex_unlock(resource_to_aquire_two);
     rsrcCnts[resource_one_id]--;
     pthread_mutex_unlock(resource_to_aquire_one);
     printf("THREAD %d done\n", threadIdx);

   pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
   int rc, safe=0;

   noWait=0;

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

   printf("Creating thread %d\n", THREAD_1);
   threadParams[THREAD_1].threadIdx=THREAD_1;
   rc = pthread_create(&threads[0], NULL, grabRsrcs, (void *)&threadParams[THREAD_1]);
   if (rc) {printf("ERROR; pthread_create() rc is %d\n", rc); perror(NULL); exit(-1);}

   if(safe) // Make sure Thread 1 finishes with both resources first
   {
     if(pthread_join(threads[0], NULL) == 0)
       printf("Thread 1 joined to main\n");
     else
       perror("Thread 1");
   }

   printf("Creating thread %d\n", THREAD_2);
   threadParams[THREAD_2].threadIdx=THREAD_2;
   rc = pthread_create(&threads[1], NULL, grabRsrcs, (void *)&threadParams[THREAD_2]);
   if (rc) {printf("ERROR; pthread_create() rc is %d\n", rc); perror(NULL); exit(-1);}

   printf("will try to join both CS threads unless they deadlock\n");

   if(!safe)
   {
     if(pthread_join(threads[0], NULL) == 0)
       printf("Thread 1 joined to main\n");
     else
       perror("Thread 1");
   }

   if(pthread_join(threads[1], NULL) == 0)
     printf("Thread 2 joined to main\n");
   else
     perror("Thread 2");

   if(pthread_mutex_destroy(&rsrcA) != 0)
     perror("mutex A destroy");

   if(pthread_mutex_destroy(&rsrcB) != 0)
     perror("mutex B destroy");

   printf("All done\n");

   exit(0);
}

static uint16_t random_number = 4;
static struct timespec a_time;
/* Psuedo RNG taken from my own code here:
    https://github.com/burinm/softsynth/blob/master/wave_function.c
*/
uint16_t rng16() {
    random_number= (uint16_t)( 74 *random_number) % (uint16_t)32771;
    a_time.tv_sec = 0;
    a_time.tv_nsec = random_number;

    return random_number;
}
