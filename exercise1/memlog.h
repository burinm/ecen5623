#include <stdint.h>
#include <string.h> //memset
#include <time.h>
#define MEMLOG_MAX  1024

typedef struct _entry {
    //Running 32bit arm, should be aligned?
    uint32_t event_id; 
    struct timespec time;
} memlog_s;

typedef struct _memlog_g {
    uint32_t index;
    memlog_s log[MEMLOG_MAX];
} memlog_g;

extern memlog_g memlog_globals;

#define MEMLOG_E_NONE           0x0
#define MEMLOG_E_S1_SCHEDULED   0x1
#define MEMLOG_E_S2_SCHEDULED   0x2
#define MEMLOG_E_S3_SCHEDULED   0x3

#define MEMLOG_E_S1_RUN         0x11 
#define MEMLOG_E_S2_RUN         0x12
#define MEMLOG_E_S3_RUN         0x13

#define MEMLOG_INIT()   memlog_globals.index = 0; \
                        for (int i=0; i<MEMLOG_MAX; i++) { \
                            memset(&(memlog_globals.log[i]), 0, sizeof(memlog_s)); \
                        }

#define MEMLOG_LOG(event, t)   memlog_globals.log[memlog_globals.index].event_id = event; \
                               clock_gettime(CLOCK_REALTIME, &memlog_globals.log[memlog_globals.index].time); \
                               memlog_globals.index++; \
                               if (memlog_globals.index == MEMLOG_MAX) { \
                                    memlog_globals.index = 0; \
                               } 

void memlog_dump();
char* memlog_event_desc(uint32_t e);

/*private*/
#define NS_PER_SEC  1000000000UL
                                  
uint64_t time_in_ns(struct timespec t);

