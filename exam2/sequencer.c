#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <limits.h>
#include <errno.h>

#include "realtime.h"
#include "timetools.h"
#include "memlog.h"

void ctrl_c(int s);
void sequencer(int v);

sem_t sem_S1;
pthread_t thread_S1;
void* S1(void* v);
memlog_t* S1_LOG;

sem_t sem_W1;
pthread_t thread_W1;
void* W1(void* v);
memlog_t* W1_LOG;

sem_t sem_W2;
pthread_t thread_W2;
void* W1(void* v);
memlog_t* W2_LOG;

pthread_attr_t rt_sched_attr;  // For realtime H/M/L threads

void* S1(void* v);
void* W1(void* v);
void* W2(void* v);



#define NUM_ITERS   30000 // 5minutes / 10ms = 30000

//#define PERIOD_NS   10000000
#define PERIOD_NS   10000000 //10ms

int running = 1;

int main() {

//Ctrl C
struct sigaction s0;
s0.sa_handler = ctrl_c;
sigaction(SIGINT, &s0, NULL);

//Catch SIGALRM and run seqencer
struct sigaction s1;
s1.sa_handler = sequencer;
sigaction(SIGALRM, &s1, NULL);

//Logging on
S1_LOG = memlog_init();
W1_LOG = memlog_init();
W2_LOG = memlog_init();

//Setup semaphores
if (sem_init(&sem_S1, 0, 0) == -1) {
    perror("Couldn't init semaphore sem_framegrab");
    exit(-1);
}

if (sem_init(&sem_W1, 0, 0) == -1) {
    perror("Couldn't init semaphore sem_framegrab");
    exit(-1);
}

if (sem_init(&sem_W2, 0, 0) == -1) {
    perror("Couldn't init semaphore sem_framegrab");
    exit(-1);
}

#if 0
if (set_main_realtime() == -1) {
    exit(-1);
}
#endif


printf("Creating S1\n");
schedule_realtime(&rt_sched_attr);
schedule_priority(&rt_sched_attr, MID_PRI);

if (pthread_create(&thread_S1, &rt_sched_attr, S1, NULL) == -1) {
    perror("Couldn't create frame grabber thread");
    exit(-1);
}

printf("Creating W1\n");
schedule_realtime(&rt_sched_attr);
schedule_priority(&rt_sched_attr, HIGH_PRI);

if (pthread_create(&thread_W1, &rt_sched_attr, W1, NULL) == -1) {
    perror("Couldn't create frame grabber thread");
    exit(-1);
}

printf("Creating W2\n");
schedule_realtime(&rt_sched_attr);
schedule_priority(&rt_sched_attr, LOW_PRI);

if (pthread_create(&thread_W1, &rt_sched_attr, W2, NULL) == -1) {
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

struct itimerspec it;
it.it_interval.tv_sec = 0;
it.it_interval.tv_nsec = PERIOD_NS; //10ms
it.it_value.tv_sec = 1; //delay 1 second to start
it.it_value.tv_nsec = 0;

if (timer_settime(timer1, 0, &it, NULL) == -1 ) {
    perror("couldn't set timer");
    exit(-1);
}


printf("Ready.\n");
pthread_join(thread_S1, NULL);
pthread_join(thread_W1, NULL);
pthread_join(thread_W2, NULL);

memlog_dump("S1.log", S1_LOG);
memlog_dump("W1.log", W1_LOG);
memlog_dump("W2.log", W2_LOG);


}


void* S1(void* v) {
    int ret = -1; 
    int count = 0;

    printf("S1 started\n");
    while(running) {

        printf("S1 waiting\n");
        ret = sem_wait(&sem_S1);

        MEMLOG_LOG(S1_LOG, MEMLOG_E_S1_RUN);
        count++;
        printf("send %d\n", count);
        //do work
        MEMLOG_LOG(S1_LOG, MEMLOG_E_S1_DONE);

        if (count > 200) {
            sem_post(&sem_W1);
            sem_post(&sem_W2);
            running = 0;
        }


        if (ret == -1) {
            perror("sem_wait sem_S1 failed");
            if (errno == EINTR) {
                return ((void*)-2); 
            }
        }
    }
return ((void*)0); 
}

void* W1(void* v) {
    int ret = -1; 

    printf("W1 started\n");
    while(running) {

        ret = sem_wait(&sem_W1);

        MEMLOG_LOG(W1_LOG, MEMLOG_E_W1_RUN);
        //do work
        MEMLOG_LOG(W1_LOG, MEMLOG_E_W1_DONE);


        if (ret == -1) {
            perror("sem_wait sem_W1 failed");
            if (errno == EINTR) {
                return ((void*)-2); 
            }
        }
    }
return ((void*)0); 
}

void* W2(void* v) {
    int ret = -1; 

    printf("W2 started\n");
    while(running) {

        ret = sem_wait(&sem_W2);
        MEMLOG_LOG(W2_LOG, MEMLOG_E_W2_RUN);
        //do work
        MEMLOG_LOG(W2_LOG, MEMLOG_E_W2_DONE);

        if (ret == -1) {
            perror("sem_wait sem_W2 failed");
            if (errno == EINTR) {
                return ((void*)-2); 
            }
        }
    }
return ((void*)0); 
}

int seq_count = 0;
void sequencer(int v) {

    //printf("Start sequencer\n");

    while(running) {
        if (seq_count % 5 == 0) { // 5 * 10 = 50ms, 20Hz
            //printf("tick\n");
            sem_post(&sem_S1);
        }

        if (seq_count % 10 == 0) { // 10 * 10 = 100ms, 10Hz
            sem_post(&sem_W1);
        }

        if (seq_count % 100 == 0) { // 100 * 10 = 1000ms, 1Hz
            sem_post(&sem_W2);
        }

        seq_count++;

        if (seq_count == 500) {
            seq_count = 0;
        }
    }

}

void ctrl_c(int s) {
    running = 0;
}
