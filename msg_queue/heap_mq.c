/* posix_mq.c - demonstrate a message Q 
    burin (c) 2020
    burin (c) 2019 - parts taken from here:
        https://github.com/burinm/aesd5013/blob/master/src/hw4/ipcs/mq.c
        https://github.com/burinm/aesd5013/blob/master/src/hw4/ipcs/message.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "mq.h"

typedef struct {
    int id;
    int len;
    void* buffer;
} image_frame_t;

#define FRAME_Q "/frame_q"
mqd_t Q;
char imagebuff[4096];

void* send_func(void *a);
void* receive_func(void *a);
void print_buf(char* b, int s);

/* catch signal */
#include <signal.h>
void ctrl_c(int addr);


int running = 1;

int main() {
    int ret_code = 0;

//install ctrl_c signal handler
struct sigaction action;
action.sa_handler = ctrl_c;
sigaction(SIGINT, &action, NULL);

struct mq_attr mq_attr = MQ_DEFAULTS;
mq_attr.mq_msgsize = sizeof(image_frame_t);

Q = mq_open(FRAME_Q, O_CREAT | O_RDWR , S_IRUSR | S_IWUSR, &mq_attr);

if (Q == (mqd_t)-1) {
    perror("Couldn't create/open global message queue\n");
    ret_code = -1;
    goto exit;
}

{ //flush queue
    printf("flushing queue\n");
    int prio;
    char b[MAX_PAYLOAD_SZ];
    struct timespec _t;
    while(1) {
            clock_gettime(CLOCK_REALTIME, &_t);
            _t.tv_sec += 2;
            int s =  mq_timedreceive(Q, b, MAX_PAYLOAD_SZ, &prio, &_t);
            if (s == 0 || errno == ETIMEDOUT) {
                break;
            }
            if (s == -1) {
                perror(NULL);
            }
            printf(".");
            fflush(stdout);
    }
    printf("\n");
}

pthread_t send_thread;
pthread_t receive_thread;

pthread_create(&send_thread, NULL, &send_func, NULL);
pthread_create(&receive_thread, NULL, &receive_func, NULL);

pthread_join(send_thread, NULL);
pthread_join(receive_thread, NULL);

exit:

mq_unlink(FRAME_Q);

return ret_code;
}

void* send_func(void *a) {
    int bytes_sent = 0;
    char b[sizeof(char*)];

    struct timespec _t;
    while(running) {
        char* p = (char*)malloc(4096);
        if (p) {
            memcpy(b, &p, sizeof(char*));
            printf("sending[]: priority = %d, length = %d buff_ptr %p\n",
                    HI_PRI, sizeof(char*), p);
            //print_buf(b, sizeof(image_frame_t));
            clock_gettime(CLOCK_REALTIME, &_t);
            _t.tv_sec += 2;
            bytes_sent = mq_timedsend(Q, b, sizeof(char*), HI_PRI, &_t);
            if (bytes_sent == -1) {
                perror("Couldn't enqueue message!\n");
                running = 0;
                break;
            }
            if (bytes_sent == 0) {
                //In this case 0 is success
            }

        } else {
            printf("Couldn't malloc buffer!\n");
        }
    }
printf("send thread exiting\n");
}

void* receive_func(void *a) {
    int prio = 0;
    int bytes_received = 0;
    char b[MAX_PAYLOAD_SZ];

    struct timespec _t;
    while(running) {

        clock_gettime(CLOCK_REALTIME, &_t);
        _t.tv_sec += 2;

        bytes_received = mq_timedreceive(Q, b, MAX_PAYLOAD_SZ, &prio, &_t);
        if (bytes_received == -1) {
            perror("Couldn't get message!\n");
            running = 0;
            break;
        }

        if (errno == ETIMEDOUT) {
            continue;
        }

        if (bytes_received > 0) {
            printf("receive[]: priority = %d, length = %d buff_ptr %p\n",
                         prio, bytes_received, b);
///print_buf(b, sizeof(image_frame_t));
            char* p;
            memcpy(&p, b, sizeof(char*));
            if (p) {
                free(p);
            }
        }
    }
printf("receive thread exiting\n");
}

void print_buf(char* b, int s) {
    for (int i=0; i < s; i++) {
        printf("0x%02x ", b[i]);
        if ((i+1) % 4 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void ctrl_c(int addr)
{
    running=0;
}

