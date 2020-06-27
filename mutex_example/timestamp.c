#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct {
    float X;
    float Y;
    float Z;
    uint64_t timestamp;
} pos_t;

pos_t location;

void* print_out_current_position(void *arg);
void* fetch_position_and_time(void *arg);
void _nav(pos_t *l);

int main() {

memset(&location, 0, sizeof(pos_t));

pthread_t position_reader;
pthread_t position_writer;

pthread_create(&position_reader, NULL, &print_out_current_position, NULL);
pthread_create(&position_writer, NULL, &fetch_position_and_time, NULL);

pthread_join(position_reader, NULL);
pthread_join(position_writer, NULL);

}

void* print_out_current_position(void *arg) {
    while(1) {
        printf("X:%f Y:%f Z:%f @ %ld\n", location.X, location.Y, location.Z, location.timestamp);
    }
}

void* fetch_position_and_time(void *arg) {

    while(1) {
        _nav(&location);
    }
}


void _nav(pos_t *l) {
    float vector = rand() % 10000 / (float)1000;
    int direction = rand() %6;
    switch (direction) {
        case 0:
            l->X += vector;
            break;
        case 1:
            l->X -= vector;
            break;
        case 2:
            l->Y += vector;
            break;
        case 3:
            l->Y -= vector;
            break;
        case 4:
            l->Z += vector;
            break;
        case 5:
            l->Z -= vector;
            break;
        default:
            printf("Akk. Plane has crashed\n");
            exit (-1);
    };

    l->timestamp++;
}
