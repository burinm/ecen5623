#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <assert.h>

#include "buffer.h"
#include "setup.h"

buffer_t buffers[NUM_BUF];

void _free_buffers(video_t *v);
void _munmap_buffers(int num_buffers);

int request_buffers(video_t *v) {
    assert(v->camera_fd != -1);

    struct v4l2_requestbuffers rb;

    memset(&rb, 0, sizeof(struct v4l2_requestbuffers));
    rb.count = NUM_BUF;  //simple ping pong strategy
    rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    rb.memory = V4L2_MEMORY_MMAP; //TODO try for DMA

    if (ioctl(v->camera_fd, VIDIOC_REQBUFS, &rb) == -1) {
        perror("Couldn't allocate buffers");
        return -1; 
    }

    //Did we get all the buffer we asked for?
    if (rb.count < NUM_BUF) {
        printf("Did not get requested amount of buffers\n");

        _free_buffers(v);
        return -1;
    }

    memset(&buffers, 0, sizeof(buffer_t) * NUM_BUF);

    //Code taken/modified from here:
    // https://www.kernel.org/doc/html/v4.9/media/uapi/v4l/mmap.html
    for (int i=0; i < rb.count; i++) {
        struct v4l2_buffer b;

        memset(&b, 0, sizeof(struct v4l2_buffer));
        b.index = i;
        b.type = rb.type;
        b.memory = rb.memory;

        if (ioctl(v->camera_fd, VIDIOC_QUERYBUF, &b) == -1) {
            printf("Couldn't get buffer info, index %d:", i);
            perror(NULL);
            _free_buffers(v);
            return -1;
        }

        buffers[i].size = b.length;
        /* From Documentation:

            https://www.kernel.org/doc/html/v4.9/media/uapi/v4l/buffer.html#c.v4l2_buffer
            .m.offset

            For the single-planar API and when memory is V4L2_MEMORY_MMAP
            this is the offset of the buffer from the start of the device
            memory. The value is returned by the driver and apart of
            serving as parameter to the mmap() function not useful for
            applications.
         */
        buffers[i].start = mmap(NULL, b.length, PROT_READ | PROT_WRITE, MAP_SHARED,
                                v->camera_fd, b.m.offset);

        if (buffers[i].start == MAP_FAILED) {
            printf("Couldn't map buffer!\n");
            deallocate_buffers(v);
            return -1;
        }

        printf("buffer #%d start=0x%p mmap=0x%u\n", i, buffers[i].start,  b.m.offset);


    }

    v->num_buffers = rb.count;
    v->type = rb.type;
    v->memory = rb.memory;
return 0;
}

void deallocate_buffers(video_t *v) {
     _munmap_buffers(v->num_buffers);
     _free_buffers(v);
}

int enqueue_buf(struct v4l2_buffer* b, int camera_fd) {
    return ioctl(camera_fd, VIDIOC_QBUF, b);
}

int dequeue_buf(struct v4l2_buffer* b, int camera_fd) {
    return ioctl(camera_fd, VIDIOC_DQBUF, b);
}




void _free_buffers(video_t *v) {
    //Try to free buffers. (count = 0) Implicit VIDIOC_STREAMOFF
    struct v4l2_requestbuffers rb;
    memset(&rb, 0, sizeof(struct v4l2_requestbuffers));
    rb.type = v->type;
    rb.memory = v->memory;

    if (ioctl(v->camera_fd, VIDIOC_REQBUFS, &rb) == -1) {
        perror("Couldn't free buffers");
    }
    printf("freed internal buffers\n");
}

void _munmap_buffers(int num_buffers) {
//TODO - Does the driver do this with VIDIOC_REQBUFS, count = 0?
for (int i=0; i < num_buffers; i++) {
        if (buffers[i].start) {
            printf("munmap buffer #%d (%p)\n", i, buffers[i].start);
            if (munmap(buffers[i].start, buffers[i].size) == -1) {
                printf("Couldn't munmap buffer #%d (%p) size %u", i, buffers[i].start, buffers[i].size);
                perror(NULL);
            }
        }
    }
}
