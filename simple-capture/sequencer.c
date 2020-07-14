#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>

#include "realtime.h"
#include "timetools.h"
#include "memlog.h"

void ctrl_c(int s);
void sequencer(int v);

sem_t sem_framegrab;
pthread_t thread_framegrab;
extern void* frame(void* v);
extern memlog_t* FRAME_LOG;

int running = 1;
int printf_on = 1;

int main() {

//Ctrl C
struct sigaction s0;
s0.sa_handler = ctrl_c;
sigaction(SIGINT, &s0, NULL);

//Catch SIGALRM and run seqencer
struct sigaction s1;
s1.sa_handler = sequencer;
sigaction(SIGALRM, &s1, NULL);

//Setup semaphores
if (sem_init(&sem_framegrab, 0, 0) == -1) {
    perror("Couldn't init semaphore sem_framegrab");
    exit(-1);
}

if (set_main_realtime() == -1) {
    exit(-1);
}

//Startup tests
long int clock_get_latency = test_clock_gettime_latency();


printf("Creating frame grabber thread\n");
pthread_attr_t rt_sched_attr;  // For realtime H/M/L threads
schedule_realtime(&rt_sched_attr);
schedule_priority(&rt_sched_attr, HIGH_PRI);

if (pthread_create(&thread_framegrab, &rt_sched_attr, frame, NULL) == -1) {
    perror("Couldn't create frame grabber thread");
    exit(-1);
}

//Interval timer for sequencer loop
timer_t timer1; // note not defined with a struct

//Install timer
if (timer_create(CLOCK_MONOTONIC, NULL, &timer1) == -1 ) {
    perror("Couldn't create timer");
    exit(-1);
}

printf("timer installed\n");

struct itimerspec it;
it.it_interval.tv_sec = 0;
it.it_interval.tv_nsec = 60000000; //60ms 
//it.it_interval.tv_nsec = 200000000; //200ms 
it.it_value.tv_sec = 1; //delay 1 second to start
it.it_value.tv_nsec = 0;

if (timer_settime(timer1, 0, &it, NULL) == -1 ) {
    perror("couldn't set timer");
    exit(-1);
}

printf("Ready.\n");
pthread_join(thread_framegrab, NULL);

//memlog_dump(FRAME_LOG);

printf("clock_gettime takes an average of %ld nsec to run\n", clock_get_latency);

}

void sequencer(int v) {
    sem_post(&sem_framegrab);
}

void ctrl_c(int s) {
    running = 0;
}
