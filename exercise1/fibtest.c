#include <stdint.h>
#include "memlog.h"

#define UINT32  uint32_t
#define FIB_LIMIT_FOR_32_BIT 47

/* Test some C1s, leave margin */
//#define C1 800000 //Approx 8 ms
#define C1 1700000 //Approx 18 ms

#define FIB_TEST(seqCnt, iterCnt, idx, jdx, fib, fib0, fib1)      \
   for(idx=0; idx < iterCnt; idx++)                               \
   {                                                              \
      fib = fib0 + fib1;                                          \
      while(jdx < seqCnt)                                         \
      {                                                           \
         fib0 = fib1;                                             \
         fib1 = fib;                                              \
         fib = fib0 + fib1;                                       \
         jdx++;                                                   \
      }                                                           \
   }                                                              \

int main() {

    memlog_t* log = memlog_init();

    for (int i=0; i < 100; i++) {
        UINT32 idx = 0, jdx = 1;
        UINT32 fib = 0, fib0 = 0, fib1 = 1;
MEMLOG_LOG(log, MEMLOG_E_FIB_TEST);
        FIB_TEST(FIB_LIMIT_FOR_32_BIT, C1, idx, jdx, fib, fib0, fib1);

    }

    memlog_gnuplot_dump(log);
    memlog_free(log);

}
