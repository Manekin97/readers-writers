#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <utime.h>
#include <errno.h>
#include <string.h>

typedef struct priority_sem_s {
    int value; // if negative, abs(sem->value) == no. of waiting threads
    pthread_mutex_t mutex;
    pthread_cond_t cv;
    int *prio_waiting;  // no. waiting (blocked) at each priority
    int *prio_released; // no. waiters released (unblocked) at each priority
    int threadsCount;
} priosem_t;

void post(priosem_t *sem);

void wait(priosem_t *sem, int prio);

void lock(pthread_mutex_t *mutex);

void unlock(pthread_mutex_t *mutex);

int prio_waiting_is_NOT_empty(priosem_t *sem);

int fetch_highest_prio_waiting(priosem_t *sem);

void cond_broadcast(pthread_cond_t *cond, pthread_mutex_t *mutex);

void cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);