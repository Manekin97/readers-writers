#include <stdlib.h> 
#include <stdio.h> 
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <utime.h>
#include <errno.h>
#include <string.h>

#define KRED  "\x1B[31m"


sem_t readersSem;
pthread_mutex_t writersSem;
int readersInside;
int readersCount;
int writersCount;


void *Reader(void *value);

void *Writer(void *value);

void Init(int readersCount, int writersCount);