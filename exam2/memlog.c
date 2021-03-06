/* memlog.c - burin (c) 2020
*/
#include <stdlib.h> //calloc, free
#include <stdio.h>
#include <stdarg.h> //vdprintf
#include <assert.h>
#include <sys/stat.h> //open
#include <fcntl.h> //open
#include <unistd.h> //close

#include "memlog.h"
#include "timetools.h"

memlog_t* memlog_init() {
    memlog_t* mem_log = (memlog_t*)calloc(1, sizeof(memlog_t));
    if (!mem_log) {
        printf("Error: couldn't allocate memory\n");
        exit(-1);
    }
return mem_log;
}

void memlog_free(memlog_t* m) {
    if (m) {
        free(m);
    }
}

inline void MEMLOG_LOG(memlog_t* l, uint32_t event) {
    assert(l->index < MEMLOG_MAX); //TODO remove assert
    l->log[l->index].event_id = event;
    clock_gettime(CLOCK_MONOTONIC, &l->log[l->index].time);
    l->index++;
    if (l->index >= MEMLOG_MAX) {
        l->index = 0;
    }
}

inline void MEMLOG_LOG24(memlog_t* l, uint32_t event, uint32_t data) {

    assert(data < 0x1000000);
    assert(event >= MEMLOG_E_S1_DATA_24);
    assert(l->index < MEMLOG_MAX); //TODO remove assert

    l->log[l->index].event_id = MEMLOG_ENCODE24(event, data);
    clock_gettime(CLOCK_MONOTONIC, &l->log[l->index].time);
    l->index++;
    if (l->index >= MEMLOG_MAX) {
        l->index = 0;
    }
}


void memlog_dump(char* f, memlog_t* l) {
    struct timespec t_prev = l->log[0].time; //TODO, problem if first entry is MEMLOG_E_NONE
    struct timespec diff;
    struct timespec t;
    int not_increasing = 0;

    int fd;
    if ((fd = open(f, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH)) == -1) {
        perror("log error");
        return;
    }

    for (int i=0; i < MEMLOG_MAX; i++) {
        if (MEMLOG_ID(l->log[i].event_id) > MEMLOG_E_NONE) {

            //timespec_subtract is destructive, save previous
            t = l->log[i].time;

            // Without the correct formatting, overflows vargs!
            // https://stackoverflow.com/questions/8304259/formatting-struct-timespec
            
            not_increasing = timespec_subtract(&diff, &l->log[i].time, &t_prev);

            dprintf(fd, "%lld.%.9ld %-30s", (long long)l->log[i].time.tv_sec,
                                  l->log[i].time.tv_nsec,
                                  memlog_event_desc(MEMLOG_ID(l->log[i].event_id)));

            dprintf(fd, "diff = %lld.%.9ld", (long long)diff.tv_sec, diff.tv_nsec);

            if (MEMLOG_ID(l->log[i].event_id) >= MEMLOG_E_S1_DATA_24) {
                dprintf(fd, " data:%u\n", MEMLOG_DATA24(l->log[i].event_id));
            } else {
                dprintf(fd, "%s", not_increasing == 1 ? " *not in order" : "");
                dprintf(fd, "\n");
            }

            t_prev = t;
        }
    }
    close(fd);
}

void memlog_gnuplot_dump(memlog_t* l) {
    uint64_t t = 0;

    for (int i=0; i < MEMLOG_MAX; i++) {
        if (l->log[i].event_id > MEMLOG_E_NONE) {
            t = time_in_us(l->log[i].time);
            printf("%.11lld %d\n", t, l->log[i].event_id);
        }
    }
}

char* memlog_event_desc(uint32_t e) {
    switch(e) {
        case MEMLOG_E_NONE:
            return "NONE";
            break;

        case MEMLOG_E_S1_RUN:
            return "MEMLOG_E_S1_RUN";
            break;

        case MEMLOG_E_W1_RUN:
            return "MEMLOG_E_S2_RUN";
            break;

        case MEMLOG_E_W2_RUN:
            return "MEMLOG_E_S3_RUN";
            break;

        case MEMLOG_E_SEQUENCER:
            return "SEQUENCER_MARK";
            break;



        case MEMLOG_E_S1_DONE:
            return "MEMLOG_E_S1_DONE";
            break;

        case MEMLOG_E_W1_DONE:
            return "MEMLOG_E_W1_DONE";
            break;

        case MEMLOG_E_W2_DONE:
            return "MEMLOG_E_W2_DONE";
            break;


        case MEMLOG_E_WCET_START:
            return "MEMLOG_E_WCET_START";
            break;

        case MEMLOG_E_WCET_DONE:
            return "MEMLOG_E_WCET_DONE";
            break;

        case MEMLOG_E_ERROR_SCAN:
            return "MEMLOG_E_ERROR_SCAN";
            break;

        case MEMLOG_E_S1_DATA_24:
            return "MEMLOG_E_ADATA";
            break;

        case MEMLOG_E_W1_DATA_24:
            return "MEMLOG_E_BDATA";
            break;

        case MEMLOG_E_W2_DATA_24:
            return "MEMLOG_E_CDATA";
            break;

        case MEMLOG_E_FIB_TEST:
            return "FIBTEST_MARK";
            break;
    };

return "Event error";
}

uint64_t time_in_us(struct timespec t) {
    return (t.tv_sec * US_PER_SEC + t.tv_nsec / 1000);
}
