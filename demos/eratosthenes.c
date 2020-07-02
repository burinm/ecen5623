// eratosthenes.c  - burin 7/1/2020 - Exam #1 
// I modified this code for the quiz
// https://github.com/burinm/ecen5623/blob/master/mutex_example/timestamp.c

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>

#define NUM_THREADS 4
void* invalidate(void *arg);

typedef struct _threadArgs
{
    int idx;
    int start;
    int end;
} threadArgsType;

threadArgsType threadarg[NUM_THREADS];
pthread_t threads[NUM_THREADS];

pthread_mutex_t prime_array_mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK() pthread_mutex_lock(&prime_array_mutex)
#define UNLOCK() pthread_mutex_unlock(&prime_array_mutex)

int total = 0;

#define IS_PRIME    1
#define NOT_PRIME   0 
int isprimes[100 * NUM_THREADS];

int main() {

for (int j=0; j<100 * NUM_THREADS; j++) {
    isprimes[j] = IS_PRIME;
}

#if 0
//Test single threaded version
    threadarg[0].idx=0;
    threadarg[0].start=1;
    threadarg[0].end=400;
    pthread_create(&threads[0], (void *)0, invalidate, (void *)&threadarg[0]);
    pthread_join(threads[0], NULL);
#endif //works!

#if 1
//for (int i=0; i< NUM_THREADS; i++) {
    threadarg[0].idx=0;
    threadarg[0].start=2;
    threadarg[0].end= 99;

    pthread_create(&threads[0], (void *)0, invalidate, (void *)&threadarg[0]);

    threadarg[1].idx=1;
    threadarg[1].start=100;
    threadarg[1].end= 168;

    pthread_create(&threads[1], (void *)0, invalidate, (void *)&threadarg[1]);

    threadarg[2].idx=2;
    threadarg[2].start=169;
    threadarg[2].end=224;

    pthread_create(&threads[2], (void *)0, invalidate, (void *)&threadarg[2]);

    threadarg[3].idx=3;
    threadarg[3].start=225;
    threadarg[3].end=400;

    pthread_create(&threads[3], (void *)0, invalidate, (void *)&threadarg[3]);

//}
#endif


#if 1
for (int i=0; i< NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
}
#endif

for (int j=0; j<100 * NUM_THREADS; j++) {
    if (isprimes[j] == IS_PRIME) {
        printf("%lu,", j);
    }
}

}

void* invalidate(void *args) {

    threadArgsType *thargs=(threadArgsType *)args;
    int start = thargs->start;
    int end = thargs->end;
    printf("Thread #%d started start = %d end = %d\n", thargs->idx, start, end);

    for (int i=start; i<= end; i++) {
        if (i < 2) continue; //algorithm
LOCK(); //Yeah, it's a large lock - optimize later
        if (isprimes[i] == IS_PRIME) {
            for (int j=pow(i,2); j <= end; j +=i) {
//printf("i = %u j = %u\n", i, j);
                isprimes[j] = NOT_PRIME;
            }
        }
UNLOCK();
    }    
}
