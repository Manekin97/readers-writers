#include "queueState.h"
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utime.h>

int readers, writers, active_writers, readersInQ, writersInQ;
pthread_mutex_t m;
pthread_cond_t writersQ, readersQ;

void* Reader_r(void* value);

void* Writer_r(void* value);

void Init_r(int readersCount, int writersCount);