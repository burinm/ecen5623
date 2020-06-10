/****************************************************************************/
/*                                                                          */
/* Sam Siewert - 2005                                                       */ 
/*		                                                                    */
/* Example #0                                                               */
/*
/* LCM = 30
/*
/* S1: T1=2,  C1=1, U1=0.5
/* S2: T2=10, C2=1, U2=0.1 
/* S3: T3=15, C3=2, U3=0.13333
/*
/* Utotal = 0.73333, RM LUB = 3x(2^1/3-1)=0.78
/*
/* All times are milliseconds
/*
/****************************************************************************/

#ifdef VXWORKS
    #include "vxWorks.h"
    #include "semLib.h"
    #include "sysLib.h"
    #include "wvLib.h"

    /* Semaphores */
	#define semTake(s) semTake(s, WAIT_FOREVER)
    #define semInit(s) (s = semBCreate(SEM_Q_FIFO, SEM_EMPTY))

    /* tasks */
    #define taskSpawn(t, pri) (taskSpawn("service- ## t", pri, 0, 8192, t, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) != ERROR)

    /* task delay */
    #define clockInit(c) sysClkRateSet(c)
    #define taskDelay(d) taskDelay(d)

    /* log */
    #define LOG(id, buffer) (if(wvEvent(id, buffer, sizeof(buffer)) == ERROR) \
		                        printf("WV EVENT ERROR\n"))
#else
    #include <stdio.h>

    /* Semaphores */
    #include <semaphore.h>
    #include <stdint.h>
    #define UINT32  uint32_t
	#define semTake(s) sem_wait(&s)
    #define semInit(s) sem_init(&s, 1, 0)
    #define semGive(s) sem_post(&s)

    /* tasks */
    #include <pthread.h>
    pthread_t thread_fibS1, thread_fibS2, thread_fibS3, thread_Sequencer;
    typedef struct { int threadIdx; } threadParams_t; //from example simplethread.c code
    threadParams_t threadParam_fibS1, threadParam_fibS2, threadParam_fibS3, threadParam_Sequencer;
    #define taskSpawn(t, pri) (pthread_create(&thread_ ## t, (void *)0, &t, (void*)&threadParam_ ## t ) == 0)

    /* task delay */
    #define clockInit(c) //Do nothing
    static struct timespec remaining_time = {0, 0}; //from posix_clock.c example
    static struct timespec sleep_time = {0, 0};
    #define taskDelay(d) sleep_time.tv_sec=0; \
                         sleep_time.tv_nsec=(d * 1000); \
                         nanosleep(CLOCK_REALTIME, &remaining_time); \

    /* log */
    #include <syslog.h>
    #define LOG(id, buffer) syslog(LOG_INFO, "%s", buffer)

    /* catch signal */
    #include <signal.h>
    void ctrl_c(int addr);

#endif


#define FIB_LIMIT_FOR_32_BIT 47

#ifdef VXWORKS
    SEM_ID semS1, semS2, semS3; 
#else
    sem_t semS1, semS2, semS3; 
#endif

int abortTest = 0;
UINT32 seqIterations = FIB_LIMIT_FOR_32_BIT;
UINT32 fib1Cnt=0, fib2Cnt=0, fib3Cnt=0;
char ciMarker[]="CI";


#define FIB_TEST(seqCnt, iterCnt, idx, jdx, fib, fib0, fib1)      \
   for(idx=0; idx < iterCnt; idx++)                               \
   {                                                              \
      fib = fib0 + fib1;                                          \
      while(jdx < seqCnt)                                         \
      {                                                           \
         fib0 = fib1;                                             \
         fib1 = fib;                                              \
         fib = fib0 + fib1;                                       \
         jdx++;                                                   \
      }                                                           \
   }                                                              \


/* Iterations, 2nd arg must be tuned for any given target type
   using windview
   
   170000 <= 10 msecs on 100 MhZ Pentium - adjust as needed for C's
   
   Be very careful of WCET overloading CPU during first period of
   LCM.
   
 */
#ifdef VXWORKS
void fibS1(void)
#else
void *fibS1(void* v)
#endif
{
UINT32 idx = 0, jdx = 1; 
UINT32 fib = 0, fib0 = 0, fib1 = 1;

   while(!abortTest)
   {
	   semTake(semS1);
	   FIB_TEST(seqIterations, 17500, idx, jdx, fib, fib0, fib1);
	   fib1Cnt++;
   }
}

#ifdef VXWORKS
void fibS2(void)
#else
void *fibS2(void* v)
#endif
{
UINT32 idx = 0, jdx = 1; 
UINT32 fib = 0, fib0 = 0, fib1 = 1;

   while(!abortTest)
   {
	   semTake(semS2);
	   FIB_TEST(seqIterations, 17500, idx, jdx, fib, fib0, fib1);
	   fib2Cnt++;
   }
}

#ifdef VXWORKS
void fibS3(void)
#else
void *fibS3(void* v)
#endif
{
UINT32 idx = 0, jdx = 1;
UINT32 fib = 0, fib0 = 0, fib1 = 1;

   while(!abortTest)
   {
	   semTake(semS3);
	   FIB_TEST(seqIterations, 35000, idx, jdx, fib, fib0, fib1);
	   fib3Cnt++;
   }
}

void shutdown(void)
{
	abortTest=1;
}


#ifdef VXWORKS
void Sequencer(void)
#else
void *Sequencer(void* v)
#endif
{

  printf("Starting Sequencer\n");

  /* Just to be sure we have 1 msec tick and TOs */
  clockInit(1000);

  /* Set up service release semaphores */
  semInit(semS1);
  semInit(semS2);
  semInit(semS3);
 
  if(taskSpawn(fibS1, 21))
  {
    printf("S1 task spawned\n");
  }
  else
    printf("S1 task spawn failed\n");

  if(taskSpawn(fibS2, 22))
  {
    printf("S1 task spawned\n");
  }
  else
    printf("S1 task spawn failed\n");

  if(taskSpawn(fibS3, 23))
  {
    printf("S3 task spawned\n");
  }
  else
    printf("S3 task spawn failed\n");
   

  /* Simulate the C.I. for S1 and S2 and mark on windview and log
     wvEvent first because F10 and F20 can preempt this task!
   */
  LOG(0xC, ciMarker);

  semGive(semS1); semGive(semS2); semGive(semS3);


  /* Sequencing loop for LCM phasing of S1, S2
   */
  while(!abortTest)
  {

	  /* Basic sequence of releases after CI */
      taskDelay(2); semGive(semS1);                 /* +2  */
      taskDelay(2); semGive(semS1);                 /* +4  */
      taskDelay(2); semGive(semS1);                 /* +6  */
      taskDelay(2); semGive(semS1);                 /* +8  */
      taskDelay(2); semGive(semS1); semGive(semS2); /* +10 */
      taskDelay(2); semGive(semS1);                 /* +12 */
      taskDelay(2); semGive(semS1);                 /* +14 */
      taskDelay(1); semGive(semS3);                 /* +15 */

      taskDelay(2); semGive(semS1);                 /* +17 */
      taskDelay(2); semGive(semS1);                 /* +19 */
	  taskDelay(1); semGive(semS2);                 /* +20 */
      taskDelay(2); semGive(semS1);                 /* +22 */
      taskDelay(2); semGive(semS1);                 /* +24 */
      taskDelay(2); semGive(semS1);                 /* +26 */
      taskDelay(2); semGive(semS1);                 /* +28 */
      taskDelay(2);                                 /* +30 */
	  
	  /* back to C.I. conditions, log event first due to preemption */
      LOG(0xC, ciMarker);

	  semGive(semS1); semGive(semS2); semGive(semS3);
  }  
 

}

#ifdef VXWORKS
void start(void)
{
#else
int main()
{
    //install ctrl_c signal handler
    struct sigaction action;
    action.sa_handler = ctrl_c;
    sigaction(SIGINT, &action, NULL);
    //from https://www.gnu.org/software/libc/manual/html_node/Syslog-Example.html
    openlog ("ex0", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL0);
#endif

	abortTest=0;

	if(taskSpawn(Sequencer, 20))
	{
	  printf("Sequencer task spawned\n");
	}
	else
	  printf("Sequencer task spawn failed\n");

#ifdef VXWORKS
#else
pthread_join(thread_fibS1, NULL);
pthread_join(thread_fibS2, NULL);
pthread_join(thread_fibS3, NULL);
pthread_join(thread_Sequencer, NULL);

closelog();
#endif
}

#ifdef VXWORKS
#else
void ctrl_c(int addr)
{
    abortTest=1;
}
#endif
