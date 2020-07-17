#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>

#include "processing.h"
#include "setup.h"
#include "buffer.h"
#include "queue.h"
#include "memlog.h"

extern int running;
extern pthread_barrier_t bar_thread_inits;
extern sem_t sem_processing;

memlog_t* PROCESSING_LOG;

int _init_processing();
void _deallocate_processing();

/* This buffer can't fall behind (it will be used to select images),
    so it is the same count as the internal camera buffers.
    Simply copy the internal buffer to raw_buffers, so we
    can return the frame_descriptors to the camera driver
    asap
*/

buffer_t raw_buffers[NUM_BUF];
static int raw_index = 0;


void* processing(void* v) {
    video_t video;
    memcpy(&video, (video_t*)v, sizeof(video_t));

    PROCESSING_LOG = memlog_init();

    if (_init_processing() == -1) {
        error_unbarrier_exit(-1);
    }

    pthread_barrier_wait(&bar_thread_inits); //GO!!

    int s_ret = -1;
    struct v4l2_buffer b;
    while(running) {

        s_ret = sem_wait(&sem_processing);

        MEMLOG_LOG(PROCESSING_LOG, MEMLOG_E_S1_RUN);

        if (s_ret == -1) {
            perror("sem_wait sem_processing failed");
            _deallocate_processing();
            error_exit(-2);
        }

        if (dequeue_V42L_frame(frame_receive_Q, &b) == -1) {
            printf("[Frame Processing: dequeue error\n");
            _deallocate_processing();
            error_exit(-1);
        }

        printf("[Processing: got frame buffer #%d start=%p size=%d\n", b.index, buffers[b.index].start, buffers[b.index].size);

assert(buffers[b.index].size == raw_buffers[raw_index].size);
        memcpy((unsigned char*)raw_buffers[raw_index].start, (unsigned char*)buffers[b.index].start, buffers[b.index].size);
        printf("[Processing: frame copied\n");

        //Requeue internal buffer - TODO - do I need to clear it?
        if (enqueue_buf(&b, video.camera_fd) == -1) {
            _deallocate_processing();
            error_exit(-1);
        }
        printf("[Processing: reenqueued frame %d\n", b.index);

#if 1 //too slow!??
        if (raw_index %3 == 0) { //TODO - testing, just write out every 3th frame
            if (enqueue_P(writeout_Q, &raw_buffers[raw_index]) == -1) {
                _deallocate_processing();
                error_exit(-1);
            }
        }
#endif

        //For now, ghetto circular buffer for testing
        raw_index++;
        if (raw_index == NUM_BUF) {
            raw_index = 0;
        }
    }

printf("[Processing: normal exit]\n");
return 0;
}

int _init_processing() {
    for (int i=0; i < NUM_BUF; i++) {
        if (allocate_frame_buffer(&raw_buffers[i]) == -1)  {
            _deallocate_processing();
            return -1;
        }
    }
return 0;
}

void _deallocate_processing() {
    for (int i=0; i < NUM_BUF; i++) {
        deallocate_buffer(&raw_buffers[i]);
    }
}
