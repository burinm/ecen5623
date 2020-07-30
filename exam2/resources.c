#include <stdlib.h>

#include "resources.h"

//Queues

//This queue passes ints
queue_container_t W1_Q = {
    .name = "/w1q",
    .max_payload_size = sizeof(int),
    .num_elems = EXAM2_Q_SZ,
    .b = NULL
};

queue_container_t W2_Q = {
    .name = "/w2q",
    .max_payload_size = sizeof(int),
    .num_elems = EXAM2_Q_SZ,
    .b = NULL
};
