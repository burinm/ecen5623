 #include <stdlib.h> //calloc, free
#include <stdio.h>
#include "memlog.h"

memlog_t* memlog_init() {
    //TODO - any error checking lol
    return ((memlog_t*)calloc(1, sizeof(memlog_t)));
}

void memlog_free(memlog_t* m) {
    if (m) {
        free(m);
    }
}

void memlog_dump(memlog_t* l) {
    uint64_t t = 0;
    uint64_t t_prev = time_in_ns(l->log[0].time); //TODO, problem if first entry is MEMLOG_E_NONE

    for (int i=0; i < MEMLOG_MAX; i++) {
        if (l->log[i].event_id > MEMLOG_E_NONE) {
            t = time_in_ns(l->log[i].time);
            //Use llx, or else 64 bit number overflows printf vargs
    /*
            printf("%0.11d.%0.9d [%s]\n", l->log[i].time.tv_sec,
                                      l->log[i].time.tv_nsec,
                                      memlog_event_desc(l->log[i].event_id));
    */
            printf("%0.11lld [%s]", t,
                                      memlog_event_desc(l->log[i].event_id));
            printf("%s", (t < t_prev) ? " *not monotonic\n" : "\n");
            t_prev = t;
        }
    }
}

void memlog_gnuplot_dump(memlog_t* l) {
    uint64_t t = 0;

    for (int i=0; i < MEMLOG_MAX; i++) {
        if (l->log[i].event_id > MEMLOG_E_NONE) {
            t = time_in_ns(l->log[i].time);
            printf("%0.11lld %d\n", t, l->log[i].event_id);
        }
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
