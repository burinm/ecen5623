#ifndef __CAPTURE_H__
#define __CAPTURE_H__

#include "setup.h"
#include "buffer.h"

#define SCAN_BUF_SIZE 30

extern buffer_t scan_buffer[SCAN_BUF_SIZE];
extern int scan_buffer_index;

#define ERROR_FULL_INIT 4
#define ERROR_LEVEL_3   3
#define ERROR_LEVEL_2   2
#define ERROR_LEVEL_1   1
#define ERROR_LEVEL_0   0

void video_error_cleanup(int state, video_t *v);

int camera_setup_check(video_t *v);
//int camera_buffers_init(video_t *v);

int camera_check_init(video_t *v);
int camera_init_internal_buffers(video_t *v);

int allocate_other_buffers();
void deallocate_other_buffers();

void* frame(void* v);


#endif
