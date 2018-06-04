#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utime.h>

typedef struct priority_sem_s {
	int value;	//	If negative its absolute value represents the amount of threads waiting on the semaphore
	pthread_mutex_t mutex;
	pthread_cond_t cv;
	int* prio_waiting;	//	Array holding the number of threads waiting at each priority
	int* prio_released; // Array holding the number of threads realeased at each priority
	int threadsCount;
} priosem_t;

int Post(priosem_t* sem);

int Wait(priosem_t* sem, int prio);

int Lock(pthread_mutex_t* mutex);

int Unlock(pthread_mutex_t* mutex);

int IsThreadWaiting(priosem_t* sem);

int GetHighestWaitingPriority(priosem_t* sem);

int Cond_broadcast(pthread_cond_t* cond, pthread_mutex_t* mutex);

int Cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex);