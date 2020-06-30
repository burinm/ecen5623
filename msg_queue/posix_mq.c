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
#include <assert.h>

#define GLOBAL_Q "/global_q"
#define MAX_QUEUE_SIZE 10
#define MAX_PAYLOAD_SZ 255 

#define HI_PRI  0 //TODO - research this
#define NO_TIMEOUT  NULL

#define MQ_DEFAULTS { \
    .mq_flags = 0, \
    .mq_maxmsg =  MAX_QUEUE_SIZE, \
    .mq_msgsize = MAX_PAYLOAD_SZ, \
    .mq_curmsgs = 0}

mqd_t Q;

void* send_func(void *a);
void* receive_func(void *a);

int running = 1;
const char canned_msg[] = "this is a test, and only a test, in the event of a real emergency, you would be instructed ...";

char* _m_buffer = NULL;

int main() {
    int ret_code = 0;

printf("-->%d\n", sizeof(canned_msg));
printf("-->%d\n", sizeof(canned_msg));

struct mq_attr mq_attr = MQ_DEFAULTS;
mq_attr.mq_msgsize = sizeof(canned_msg);

Q = mq_open(GLOBAL_Q, O_CREAT | O_RDWR , S_IRUSR | S_IWUSR, &mq_attr);

if (Q == (mqd_t)-1) {
    perror("Couldn't create/open global message queue\n");
    ret_code = -1;
    goto exit;
}

_m_buffer = malloc(sizeof(canned_msg));
assert(_m_buffer);

pthread_t send_thread;
pthread_t receive_thread;

pthread_create(&send_thread, NULL, &send_func, NULL);
pthread_create(&receive_thread, NULL, &receive_func, NULL);

pthread_join(send_thread, NULL);
pthread_join(receive_thread, NULL);

exit:

if (_m_buffer) {
    free(_m_buffer);
}
mq_unlink(GLOBAL_Q);

return ret_code;
}

void* send_func(void *a) {
    int bytes_sent = 0;
    while(running) {
        bytes_sent = mq_send(Q, canned_msg, sizeof(canned_msg), HI_PRI);
        if (bytes_sent == -1) {
            perror("Couldn't enqueue message!\n");
            running = 0;
            break;
        }
    }
}

void* receive_func(void *a) {
    int bytes_received = 0;
    while(running) {
        bytes_received = mq_receive(Q, _m_buffer,  sizeof(canned_msg), HI_PRI);
        if (bytes_received == -1) {
            perror("Couldn't get message!\n");
            running = 0;
            break;
        }
        if (bytes_received > 0) {
            printf("[%s]", _m_buffer);
        }
    }
}
