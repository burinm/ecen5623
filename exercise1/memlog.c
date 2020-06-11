#include <stdio.h>
#include "memlog.h"

memlog_g memlog_globals;

void memlog_dump() {
    for (int i=0; i < MEMLOG_MAX; i++) {
        uint64_t t = time_in_ns(memlog_globals.log[i].time);        
        //Use llx, or else 64 bit number overflows printf vargs
/*
        printf("%0.11d.%0.9d [%s]\n", memlog_globals.log[i].time.tv_sec,
                                  memlog_globals.log[i].time.tv_nsec,
                                  memlog_event_desc(memlog_globals.log[i].event_id));
*/
        printf("%0.11lld [%s]\n", t, 
                                  memlog_event_desc(memlog_globals.log[i].event_id));
    }
}

char* memlog_event_desc(uint32_t e) {
    switch(e) {
        case MEMLOG_E_NONE:
            return "NONE";
            break;

        case MEMLOG_E_S1_SCHEDULED:
            return "MEMLOG_E_S1_SCHEDULED";
            break;

        case MEMLOG_E_S2_SCHEDULED:
            return "MEMLOG_E_S2_SCHEDULED";
            break;

        case MEMLOG_E_S3_SCHEDULED:
            return "MEMLOG_E_S3_SCHEDULED";
            break;

        case MEMLOG_E_S1_RUN:
            return "MEMLOG_E_S1_RUN";
            break;

        case MEMLOG_E_S2_RUN:
            return "MEMLOG_E_S2_RUN";
            break;

        case MEMLOG_E_S3_RUN:
            return "MEMLOG_E_S3_RUN";
            break;
    };

return "Event error";
}

uint64_t time_in_ns(struct timespec t) {
    return (t.tv_sec * NS_PER_SEC + t.tv_nsec);
}
