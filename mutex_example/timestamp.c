#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define NAV_CLOCK   CLOCK_MONOTONIC

typedef struct {
    double X;
    double Y;
    double Z;
    double Roll;
    double Pitch;
    double Yaw;
    struct timespec timestamp;
} pos_t;

pos_t location;

void* print_out_current_position(void *arg);
void* fetch_position_and_time(void *arg);
void _nav(pos_t *l);

pthread_mutex_t location_mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_LOCATION() pthread_mutex_lock(&location_mutex)
#define UNLOCK_LOCATION() pthread_mutex_unlock(&location_mutex)

int main() {

memset(&location, 0, sizeof(pos_t));

pthread_t position_reader;
pthread_t position_writer;

struct timespec tmp_time; 
clock_getres(NAV_CLOCK, &tmp_time);
printf("Clock resolution is %lu.%09uns\n", tmp_time.tv_sec, tmp_time.tv_nsec);

pthread_create(&position_reader, NULL, &print_out_current_position, NULL);
pthread_create(&position_writer, NULL, &fetch_position_and_time, NULL);

pthread_join(position_reader, NULL);
pthread_join(position_writer, NULL);

}

void* print_out_current_position(void *arg) {
    while(1) {
        LOCK_LOCATION();
            printf("X:%f Y:%f Z:%f R:%f P:%f Y:%f @ %lu.%09u\n", location.X, location.Y, location.Z,
                                location.Roll, location.Pitch, location.Yaw,
                                location.timestamp.tv_sec, location.timestamp.tv_nsec);
        UNLOCK_LOCATION();
    }
}

void* fetch_position_and_time(void *arg) {

    while(1) {
        LOCK_LOCATION();
            _nav(&location);
        UNLOCK_LOCATION();
    }
}


static struct timespec nav_time_temp;

void _nav(pos_t *l) {
    double vector = rand() % 100000 / (double)1000000;
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

    l->Roll = 5;
    l->Pitch = .05;
    l->Yaw = .012345;

    clock_gettime(NAV_CLOCK, &nav_time_temp);
    l->timestamp.tv_sec = nav_time_temp.tv_sec;
    l->timestamp.tv_nsec = nav_time_temp.tv_nsec;
}
