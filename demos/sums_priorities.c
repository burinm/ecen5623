/* burin 6/1/2020 

    I modified this code for the quiz
     https://github.com/burinm/ecen5623/blob/master/mutex_example/timestamp.c

    I modified the quiz code and combined it with pthread3ok.c
     from Exercise #3
*/

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_PROCESSORS  4 //Raspberry Pi 3b+

void* sum_1_299(void *arg);
void* sum_1_99(void *arg);
void* sum_100_199(void *arg);
void* sum_200_299(void *arg);

struct sched_param rt_param;
pthread_attr_t rt_sched_attr;  // For realtime

#define HIGH_PRIORITY (rt_max_prio -1)
#define MED_PRIORITY  (rt_max_prio -2)
#define LOW_PRIORITY  (rt_max_prio -3)


pthread_mutex_t location_mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK() pthread_mutex_lock(&location_mutex)
#define UNLOCK() pthread_mutex_unlock(&location_mutex)

int total = 0;

int main() {

cpu_set_t threadcpu;

int rt_max_prio = sched_get_priority_max(SCHED_FIFO);
int rt_min_prio = sched_get_priority_min(SCHED_FIFO);

pthread_t thread_1_99;
pthread_t thread_100_199;
pthread_t thread_200_299;

//Setup for affinity
   int coreid=0;
   CPU_ZERO(&threadcpu);
   printf("Setting affinity to core %d\n", coreid);
   CPU_SET(coreid, &threadcpu);

   for(int i=0; i < NUM_PROCESSORS; i++) {
       if(CPU_ISSET(i, &threadcpu))  printf(" CPU-%d ", i);
   }

//Setup realtime threads' schedule policy
   if (pthread_attr_init(&rt_sched_attr) != 0) {
    perror("pthread_attr_init");
    exit(-1);
   }

   if (pthread_attr_setinheritsched(&rt_sched_attr, PTHREAD_EXPLICIT_SCHED) != 0) {
    perror("pthread_attr_setinheritsched");
    exit(-1);
   }

   if (pthread_attr_setschedpolicy(&rt_sched_attr, SCHED_FIFO) != 0) {
    perror("pthread_attr_setschedpolicy");
    exit(-1);
   }

//Also set affinity so the example will correctly run AMP
   if (pthread_attr_setaffinity_np(&rt_sched_attr, sizeof(cpu_set_t), &threadcpu) != 0) {
    perror("pthread_attr_setaffinity_np");
    exit(-1);
   }



//Thread 1
rt_param.sched_priority = HIGH_PRIORITY;
pthread_attr_setschedparam(&rt_sched_attr, &rt_param);

pthread_create(&thread_1_99, NULL, &sum_1_99, NULL);

//Thread 2
rt_param.sched_priority = MED_PRIORITY;
pthread_attr_setschedparam(&rt_sched_attr, &rt_param);

pthread_create(&thread_100_199, NULL, &sum_100_199, NULL);

//Thread 3
rt_param.sched_priority = LOW_PRIORITY;
pthread_attr_setschedparam(&rt_sched_attr, &rt_param);

pthread_create(&thread_200_299, NULL, &sum_200_299, NULL);

//Join all threads
pthread_join(thread_1_99, NULL);
pthread_join(thread_100_199, NULL);
pthread_join(thread_200_299, NULL);

printf("total = %d\n", total);

}

void* sum_1_99(void *arg) {
    printf("sum_1_99 enter\n");
    int sum = 0;
    for (int i=1; i <=99; i++) {
        sum += i;
    }
    LOCK();
        total += sum;
    UNLOCK();

    printf("sum_1_99 exit %d\n", sum);
}

void* sum_100_199(void *arg) {
    printf("sum_100_199 enter\n");
    int sum = 0;
    for (int i=100; i <=199; i++) {
        sum += i;
    }
    LOCK();
        total += sum;
    UNLOCK();

    printf("sum_100_199 exit %d\n", sum);
}

void* sum_200_299(void *arg) {
    printf("sum_200_299 enter\n");
    int sum = 0;
    for (int i=200; i <=299; i++) {
        sum += i;
    }
    LOCK();
        total += sum;
    UNLOCK();

    printf("sum_200_299 exit %d\n", sum);
}
