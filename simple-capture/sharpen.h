#ifndef __SHARPEN_H__
#define __SHARPEN_H__

#include "setup.h" 
#include "buffer.h"
#include "dumptools.h" 

#define SHARPEN_ROWS    3
#define SHARPEN_COLS    3
#define SHARPEN_SIZE    (SHARPEN_ROWS * SHARPEN_COLS)
//Assuming filter is square
#define FILTER_RANGE    (SHARPEN_COLS / 2)


/*TODO - should only need column number of rows for buffer,
            circular buffer like pointers, and load one
            new row at a time
extern unsigned char sharpen_buffer[SHARPEN_COLS * X_RES];
*/

#define SHARPEN_BUF_SIZE   FRAME_SIZE 
extern buffer_t sharpen_buffer;
extern float SHARPEN_FLT[SHARPEN_SIZE];

#define SHARPEN_WIKIPEDIA_EXAMPLE   {  0.0, -1.0,  0.0, \
                                      -1.0,  5.0, -1.0, \
                                       0.0, -1.0,  0.0, }

//TODO deallocate sharpen buffer
int init_sharpen_buffer(buffer_t* b);
void print_sharpen_filter();
void sharpen(buffer_t *src, buffer_t* dst, size_t offset);

#endif
