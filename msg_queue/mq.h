/* mq.h - demonstrate a message Q 
    burin (c) 2020
    burin (c) 2019 - parts taken from here:
        https://github.com/burinm/aesd5013/blob/master/src/hw4/ipcs/mq.c
        https://github.com/burinm/aesd5013/blob/master/src/hw4/ipcs/message.c
*/

#define MAX_QUEUE_SIZE 10
#define MAX_PAYLOAD_SZ 128

#define HIGHEST_PRI  (sysconf(_SC_MQ_PRIO_MAX) - 1) 
#define HI_PRI     30
#define NO_TIMEOUT  NULL

#define MQ_DEFAULTS { \
    .mq_flags = 0, \
    .mq_maxmsg =  MAX_QUEUE_SIZE, \
    .mq_msgsize = MAX_PAYLOAD_SZ, \
    .mq_curmsgs = 0}
