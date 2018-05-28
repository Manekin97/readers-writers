#include "queueState.h"
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utime.h>

pthread_mutex_t m;
pthread_cond_t writersQ, readersQ;
int readers, writers, readersInQ, writersInQ;;

void* Reader(void* value);

void* Writer(void* value);

void Init(int readersCount, int writersCount);