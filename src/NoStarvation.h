#include "PriorityQueue.h"
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

sem_t resourceAccess; // controls access (read/write) to the resource
sem_t readCountAccess; // for syncing changes to shared variable readCount
priosem_t serviceQueue; // FAIRNESS: preserves ordering of requests (signaling must be FIFO)

void* Reader_o(void* value);

void* Writer_o(void* value);

void Init_o(int readersCount, int writersCount);