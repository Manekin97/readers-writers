#include <stdlib.h> 
#include <stdio.h> 
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <utime.h>
#include <errno.h>
#include <string.h>

#define KRED  "\x1B[31m"

int readersCount;
int writersCount;

int readcount, writecount;  //  (initial value = 0)
sem_t readTry, resource;    //  (initial value = 1)
pthread_mutex_t rmutex, wmutex;

void *Reader_r(void *value);

void *Writer_r(void *value);

void Init_r(int readersCount, int writersCount);


// sem_t readersSem, rpeekSem, wpeekSem, restReadersSem;
// pthread_mutex_t writersSem;

// int writersInside;
// int readersInside;

// int readersCount;
// int writersCount;


// void *Reader_r(void *value);

// void *Writer_r(void *value);

// void Init_r(int readersCount, int writersCount);