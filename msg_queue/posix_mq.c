/* posix_mq.c - demonstrate a message Q 
    burin (c) 2020
    burin (c) 2019 - parts taken from here:
        https://github.com/burinm/aesd5013/blob/master/src/hw4/ipcs/mq.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <pthread.h>

#define GLOBAL_Q "/global_q"
#define MAX_QUEUE_SIZE 10
#define MAX_PAYLOAD_SZ 10

#define MQ_DEFAULTS { \
    .mq_flags = 0, \
    .mq_maxmsg =  MAX_QUEUE_SIZE, \
    .mq_msgsize = MAX_PAYLOAD_SZ, \
    .mq_curmsgs = 0}

mqd_t Q;

void* send_func(void *a);
void* receive_func(void *a);

int running = 1;

int main() {

struct mq_attr mq_attr = MQ_DEFAULTS;
Q = mq_open(GLOBAL_Q, O_CREAT | O_RDWR , S_IRUSR | S_IWUSR, &mq_attr);

if (Q == (mqd_t)-1) {
    perror("Couldn't create/open global message queue\n");
    exit(-1);
}

pthread_t send_thread;
pthread_t receive_thread;

pthread_create(&send_thread, NULL, &send_func, NULL);
pthread_create(&receive_thread, NULL, &receive_func, NULL);

pthread_join(send_thread, NULL);
pthread_join(receive_thread, NULL);

mq_unlink(GLOBAL_Q);
}

void* send_func(void *a) {
    while(running) {
        printf("(s)");
    }
}

void* receive_func(void *a) {
    while(running) {
        printf("(r)");
    }
}
