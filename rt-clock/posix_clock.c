/****************************************************************************/
/* Function: nanosleep and POSIX 1003.1b RT clock demonstration             */
/*                                                                          */
/* Sam Siewert - 02/05/2011                                                 */
/*                                                                          */
/****************************************************************************/
#define _GNU_SOURCE

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#include <sched.h>

#define RUN_RT_THREAD
#define USE_AFFINITY

#define NSEC_PER_SEC (1000000000)
#define DELAY_TICKS (1)
#define ERROR (-1)
#define OK (0)

void end_delay_test(void);

static struct timespec sleep_time = {0, 0};
static struct timespec sleep_requested = {0, 0};
static struct timespec remaining_time = {0, 0};

static unsigned int sleep_count = 0;

pthread_t main_thread;
pthread_attr_t main_sched_attr;
int rt_max_prio, rt_min_prio, min;
struct sched_param main_param;


void print_scheduler(void)
{
   int schedType;

   schedType = sched_getscheduler(getpid());

   switch(schedType)
   {
     case SCHED_FIFO:
           printf("Pthread Policy is SCHED_FIFO\n");
           break;
     case SCHED_OTHER:
           printf("Pthread Policy is SCHED_OTHER\n");
       break;
     case SCHED_RR:
           printf("Pthread Policy is SCHED_OTHER\n");
           break;
     default:
       printf("Pthread Policy is UNKNOWN\n");
   }
}


int delta_t(struct timespec *stop, struct timespec *start, struct timespec *delta_t)
{
  int dt_sec=stop->tv_sec - start->tv_sec;
  int dt_nsec=stop->tv_nsec - start->tv_nsec;

  if (dt_sec < 0)
  {
    return (ERROR);
  }

  if (dt_sec == 0 && dt_nsec < 0)
  {
    return (ERROR);
  }

  if(dt_nsec >= 0)
  {
    delta_t->tv_sec=dt_sec;
    delta_t->tv_nsec=dt_nsec;
  }
  else
  {
    delta_t->tv_sec=dt_sec-1;
    delta_t->tv_nsec=NSEC_PER_SEC - dt_nsec;
  }

  return(OK);
}

static struct timespec rtclk_dt = {0, 0};
static struct timespec rtclk_start_time = {0, 0};
static struct timespec rtclk_stop_time = {0, 0};
static struct timespec delay_error = {0, 0};

void *delay_test(void *threadID)
{
  int i;
  unsigned int max_sleep_calls=3;
  int flags = 0;
  struct timespec rtclk_resolution;

  sleep_count = 0;

  if(clock_getres(CLOCK_REALTIME, &rtclk_resolution) == ERROR)
  {
      perror("clock_getres");
      exit(-1);
  }
  else
  {
      printf("\n\nPOSIX Clock demo using system RT clock with resolution:\n\t%ld secs, %ld microsecs, %ld nanosecs\n", rtclk_resolution.tv_sec, (rtclk_resolution.tv_nsec/1000), rtclk_resolution.tv_nsec);
  }

  /* run test for 3 seconds */
  sleep_time.tv_sec=3;
  sleep_time.tv_nsec=0;
  sleep_requested.tv_sec=sleep_time.tv_sec;
  sleep_requested.tv_nsec=sleep_time.tv_nsec;

  /* start time stamp */ 
  clock_gettime(CLOCK_REALTIME, &rtclk_start_time);

  /* request sleep time and repeat if time remains */
  do 
  {
      nanosleep(&sleep_time, &remaining_time);
   clock_gettime(CLOCK_REALTIME, &rtclk_stop_time); //Put as close as possible to end of nanosleep
         
      sleep_time.tv_sec = remaining_time.tv_sec;
      sleep_time.tv_nsec = remaining_time.tv_nsec;
      sleep_count++;
  } 
  while (((remaining_time.tv_sec > 0) || (remaining_time.tv_nsec > 0)) && (sleep_count < max_sleep_calls));


  delta_t(&rtclk_stop_time, &rtclk_start_time, &rtclk_dt);
  delta_t(&rtclk_dt, &sleep_requested, &delay_error);

  end_delay_test();

}

void end_delay_test(void)
{
  printf("\n");
  printf("RT clock start seconds = %ld, nanoseconds = %ld\n", 
         rtclk_start_time.tv_sec, rtclk_start_time.tv_nsec);

  printf("RT clock stop seconds = %ld, nanoseconds = %ld\n", 
         rtclk_stop_time.tv_sec, rtclk_stop_time.tv_nsec);

  printf("RT clock DT seconds = %ld, nanoseconds = %ld\n", 
         rtclk_dt.tv_sec, rtclk_dt.tv_nsec);

  printf("Requested sleep seconds = %ld, nanoseconds = %ld\n", 
         sleep_requested.tv_sec, sleep_requested.tv_nsec);

  printf("\n");
  printf("Sleep loop count = %ld\n", sleep_count);
  printf("RT clock delay error = %ld, nanoseconds = %ld\n", 
         delay_error.tv_sec, delay_error.tv_nsec);
 
  exit(0);

}


void main(void)
{
   int rc, scope;

#ifdef USE_AFFINITY
    #define CPU_NUM   3
    #define CURRENT_PID  0

    printf("Set CPU affinity to CPU %d\n", CPU_NUM);
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

#endif

   printf("Before adjustments to scheduling policy:\n");
   print_scheduler();

#ifdef RUN_RT_THREAD
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
   }

   printf("After adjustments to scheduling policy:\n");
   print_scheduler();

   main_param.sched_priority = rt_max_prio;
   pthread_attr_setschedparam(&main_sched_attr, &main_param);

   rc = pthread_create(&main_thread, &main_sched_attr, delay_test, (void *)0);

   if (rc)
   {
       printf("ERROR; pthread_create() rc is %d\n", rc);
       perror("pthread_create");
       exit(-1);
   }

   pthread_join(main_thread, NULL);

   if(pthread_attr_destroy(&main_sched_attr) != 0)
     perror("attr destroy");
#else
   delay_test((void *)0);
#endif

   printf("TEST COMPLETE\n");
}

