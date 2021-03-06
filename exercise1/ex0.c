/*
    Sam Siewert - 2005

    burin (c) spring 2020 -  modified/ported from
      http://ecee.colorado.edu/~ecen5623/ecen/ex/Linux/code/VxWorks-sequencers/ex0.c

    Exercise 1, part #4 - ecen5623 - spring 2020

    S1: T1 = 2 C1 = 1
    S2: T2 = 5 C2 = 2

    Utotal = 90%, LUB = 82.8%

    All times are usec
*/

#define T_UNIT  10000
//TODO - These deadlines are not yet checked
#define T1 20000
#define T2 50000

//Tested in fibtest
//Both set to 10ms, because that' the minimum timeslice we are working with
#define C1 800000 //~10ms
//#define C2 1700000
#define C2 800000 //~10ms

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
    #define _GNU_SOURCE
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
    pthread_t thread_fibS1, thread_fibS2, thread_Sequencer;
    typedef struct { int threadIdx; } threadParams_t; //from example simplethread.c code
    threadParams_t threadParam_fibS1, threadParam_fibS2, threadParam_Sequencer;
    
    //#define RUN_RT_THREAD

    #ifdef RUN_RT_THREAD
        #include <sys/types.h>
        #include <unistd.h>

        //This whole part taken from posix_clock example
        int rt_max_prio, rt_min_prio;
        pthread_attr_t sched_attr_fibS1, sched_attr_fibS2, sched_attr_Sequencer;
        struct sched_param main_param;
        pthread_attr_t main_sched_attr;
        #define taskSpawn(t, pri) (pthread_create(&thread_ ## t, &sched_attr_ ## t, &t, (void*)&threadParam_ ## t ) == 0)
    #else
        #define taskSpawn(t, pri) (pthread_create(&thread_ ## t, (void *)0, &t, (void*)&threadParam_ ## t ) == 0)
    #endif

    /* task delay */
    #define clockInit(c) //Do nothing
    static struct timespec remaining_time = {0, 0}; //from posix_clock.c example
    static struct timespec sleep_time = {0, 0};
    #define taskDelay(d) sleep_time.tv_sec=0; \
                         sleep_time.tv_nsec=(d * 1000); \
                         nanosleep(&sleep_time, &remaining_time);

    /* log - currently not being used */
    #include <syslog.h>
    #define LOG(id, buffer) syslog(LOG_INFO, "%s", buffer)

    /* catch signal */
    #include <signal.h>
    void ctrl_c(int addr);

    //#define USE_AFFINITY

    #ifdef USE_AFFINITY
        #include <sched.h>
    #endif

#endif

#include "memlog.h"

//3 logs to rule them all
memlog_t* S1_LOG;
memlog_t* S2_LOG;
memlog_t* SEQ_LOG;


#define FIB_LIMIT_FOR_32_BIT 47

#ifdef VXWORKS
    SEM_ID semS1, semS2;
#else
    sem_t semS1, semS2;
#endif

int abortTest = 0;
UINT32 seqIterations = FIB_LIMIT_FOR_32_BIT;
UINT32 fib1Cnt=0, fib2Cnt=0;
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
MEMLOG_LOG(S1_LOG, MEMLOG_E_S1_RUN);
	   FIB_TEST(seqIterations, C1, idx, jdx, fib, fib0, fib1);
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
MEMLOG_LOG(S2_LOG, MEMLOG_E_S2_RUN);
	   FIB_TEST(seqIterations, C2, idx, jdx, fib, fib0, fib1);
	   fib2Cnt++;
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

  printf("# Starting Sequencer\n");

  /* Just to be sure we have 1 msec tick and TOs */
  clockInit(1000);

  /* Set up service release semaphores */
  semInit(semS1);
  semInit(semS2);
 

  if(taskSpawn(fibS1, 21))
  {
    printf("# S1 task spawned\n");
  }
  else
    printf("S1 task spawn failed\n");


  if(taskSpawn(fibS2, 22))
  {
    printf("# S2 task spawned\n");
  }
  else
    printf("S2 task spawn failed\n");


#ifdef VXWORKS
  /* Simulate the C.I. for S1 and S2 and mark on windview and log
     wvEvent first because F10 and F20 can preempt this task!
   */
  LOG(0xC, ciMarker);

  semGive(semS1); semGive(semS2);
#endif

  /* Sequencing loop for LCM phasing of S1, S2
   */
  int iters = 5;
  //int iters = 100000;
  while(!abortTest && iters)
  {

    /*
        Schedule:

        [ 1][ 2][ 3][ 4][ 5][ 6][ 7][ 8][ 9][10]
        (S1)    (S1)    (S1)    (S1)    (S1)****
            (S2)    (S2)    (S2)    (S2)    ****

        Deadlines:

        <----D1><----D1><----D1><----D1><----D1>
        <----------------D2><----------------D2>
    */


MEMLOG_LOG(SEQ_LOG, MEMLOG_E_SEQUENCER);
        //Start with delay, (looped from end of schedule)
        taskDelay(T_UNIT); semGive(semS1);          // 0-1
MEMLOG_LOG(SEQ_LOG, MEMLOG_E_SEQUENCER);
        taskDelay(T_UNIT); semGive(semS2);          // 1-2
MEMLOG_LOG(SEQ_LOG, MEMLOG_E_SEQUENCER);
        taskDelay(T_UNIT); semGive(semS1);          // 2-3
MEMLOG_LOG(SEQ_LOG, MEMLOG_E_SEQUENCER);
        taskDelay(T_UNIT); semGive(semS2);          // 3-4
MEMLOG_LOG(SEQ_LOG, MEMLOG_E_SEQUENCER);
        taskDelay(T_UNIT); semGive(semS1);          // 4-5
MEMLOG_LOG(SEQ_LOG, MEMLOG_E_SEQUENCER);
        taskDelay(T_UNIT); semGive(semS2);          // 5-6
MEMLOG_LOG(SEQ_LOG, MEMLOG_E_SEQUENCER);
        taskDelay(T_UNIT); semGive(semS1);          // 6-7
MEMLOG_LOG(SEQ_LOG, MEMLOG_E_SEQUENCER);
        taskDelay(T_UNIT); semGive(semS2);          // 7-8
MEMLOG_LOG(SEQ_LOG, MEMLOG_E_SEQUENCER);
        taskDelay(T_UNIT); semGive(semS1);          // 8-9
MEMLOG_LOG(SEQ_LOG, MEMLOG_E_SEQUENCER);
        taskDelay(T_UNIT);                          // 9-0

#ifdef VXWORKS
	  /* back to C.I. conditions, log event first due to preemption */
      LOG(0xC, ciMarker);
	  semGive(semS1); semGive(semS2); //Why?, was this for vxWorks logging anomaly ??
#endif

      iters--;
  }  
 
    abortTest = 1;
    semGive(semS1); semGive(semS2);

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
#endif

    //quick logging facility for timing
    S1_LOG = memlog_init();
    S2_LOG = memlog_init();
    SEQ_LOG = memlog_init();

	abortTest=0;

	if(taskSpawn(Sequencer, 20))
	{
	  printf("# Sequencer task spawned\n");
	}
	else
	  printf("Sequencer task spawn failed\n");

#ifdef VXWORKS
#else

#ifdef USE_AFFINITY
    #define CPU_NUM   3
    #define CURRENT_PID  0
    #include <stdlib.h> //exit

    int rc;

    printf("# Set CPU affinity to CPU %d\n", CPU_NUM);
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(CPU_NUM, &cpu_set);

    if (sched_setaffinity(CURRENT_PID, sizeof(cpu_set), &cpu_set) != 0) {
       printf("ERROR; sched_setaffinity rc is %d\n", rc);
       perror("sched_setaffinity"); exit(-1);
    }

    //Trust but verify
    cpu_set_t get_cpu_set;
    if (sched_getaffinity(CURRENT_PID, sizeof(get_cpu_set), &get_cpu_set) != 0) {
       printf("ERROR; sched_getaffinity rc is %d\n", rc);
       perror("sched_getaffinity"); exit(-1);
    }

    if (!CPU_EQUAL(&cpu_set, &get_cpu_set)) {
        printf("ERROR; CPU affinity not set\n");
        exit(-1);
    }

    int current_cpu = sched_getcpu();
    if (current_cpu == -1) {
       printf("ERROR; sched_getcpu rc is %d\n", rc);
       perror("sched_getcpu"); exit(-1);
    }

    if (current_cpu != CPU_NUM) {
        printf("ERROR; Process not running on CPU %d (using p:%d)\n", CPU_NUM, current_cpu);
        exit(-1);
    }

#ifdef RUN_RT_THREAD
   //Taken directly from posix_clock.c example
   pthread_attr_init(&main_sched_attr);
   pthread_attr_setinheritsched(&main_sched_attr, PTHREAD_EXPLICIT_SCHED);
   pthread_attr_setschedpolicy(&main_sched_attr, SCHED_FIFO);

   rt_max_prio = sched_get_priority_max(SCHED_FIFO);
   rt_min_prio = sched_get_priority_min(SCHED_FIFO);

   main_param.sched_priority = rt_max_prio;
   rc=sched_setscheduler(getpid(), SCHED_FIFO, &main_param);


   if (rc)
   {
       printf("ERROR; sched_setscheduler rc is %d\n", rc);
       perror("sched_setschduler"); exit(-1);
   } else {
       printf("# FIFO scheduling enabled\n");
   }
      

   main_param.sched_priority = rt_max_prio;
   pthread_attr_setschedparam(&main_sched_attr, &main_param);

#endif

#endif

pthread_join(thread_fibS1, NULL);
pthread_join(thread_fibS2, NULL);
pthread_join(thread_Sequencer, NULL);

#ifdef RUN_RT_THREAD //from posix_clock.c 
if(pthread_attr_destroy(&main_sched_attr) != 0)
     perror("attr destroy");
#endif


#endif

printf("# fib1Cnt=%d, fib2Cnt=%d\n", fib1Cnt, fib2Cnt);

//Dump all logs, sort outside program by column 1, timestamp
memlog_gnuplot_dump(S1_LOG);
memlog_gnuplot_dump(S2_LOG);
memlog_gnuplot_dump(SEQ_LOG);

memlog_free(S1_LOG);
memlog_free(S2_LOG);
memlog_free(SEQ_LOG);
}

#ifdef VXWORKS
#else
void ctrl_c(int addr)
{
    abortTest=1;
}
#endif
