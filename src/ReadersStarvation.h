#include "queueState.h"
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utime.h>

queueState_t* state;

int readersInside;
int writersInside;

sem_t readTry, resource;
pthread_mutex_t rmutex, wmutex;

void* Reader_r(void* value);

void* Writer_r(void* value);

void Init_r(int readersCount, int writersCount);