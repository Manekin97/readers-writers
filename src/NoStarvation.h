#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <utime.h>
#include <errno.h>
#include <string.h>
#include "PriorityQueue.h"

#define KRED "\x1B[31m"
#define KGRN  "\x1B[32m"

int readCount; // init to 0; number of readers currently accessing resource

// all semaphores initialised to 1
sem_t resourceAccess;   // controls access (read/write) to the resource
sem_t readCountAccess;  // for syncing changes to shared variable readCount
priosem_t serviceQueue; // FAIRNESS: preserves ordering of requests (signaling must be FIFO)

void *Reader_o(void *value);

void *Writer_o(void *value);

void Init_o(int readersCount, int writersCount);