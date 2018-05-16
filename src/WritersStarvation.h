#include "queueState.h"
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utime.h>

// queueState_t* state;

// sem_t readTry;
// pthread_mutex_t wmutex, rmutex;
pthread_mutex_t m;
pthread_cond_t writersQ, readersQ;

// int readersInside;
// int writersInside;
// int readersQ;
// int writersQ;

int readers, writers;


void* Reader(void* value);

void* Writer(void* value);

void Init(int readersCount, int writersCount);