#include "PriorityQueue.h"
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utime.h>

int readers, writers, readersInQ, writersInQ;;

sem_t libraryAccess;   // Controls access to the libary
sem_t readersAccess;  //  Ensures only one thread modifies readersInside variable
priosem_t serviceQueue; //  Semaphore with priority queue

void* Reader_o(void* value);

void* Writer_o(void* value);

void Init_o(int readersCount, int writersCount);