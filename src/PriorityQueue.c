#include "PriorityQueue.h"

void post(priosem_t *sem){
    lock(&(sem->mutex));
    sem->value++;

    if (sem->value <= 0 && prio_waiting_is_NOT_empty(sem)){
        // someone was waiting; release one of the highest prio
        int prio = fetch_highest_prio_waiting(sem);
        sem->prio_waiting[prio]--;
        sem->prio_released[prio]++;
        cond_broadcast(&(sem->cv), &(sem->mutex));
    }

    unlock(&(sem->mutex));
}

void wait(priosem_t *sem, int prio) {
    lock(&(sem->mutex));
    sem->value--;

    if (sem->value < 0){
        // get in line
        sem->prio_waiting[prio]++;
        while (sem->prio_released[prio] < 0){
            cond_wait(&(sem->cv), &(sem->mutex));
        }

        // ok to leave
        sem->prio_released[prio]--;
    }

    unlock(&(sem->mutex));
}

void lock(pthread_mutex_t *mutex) {
    if(pthread_mutex_lock(mutex) == -1) {
        printf("%s", strerror(errno));                
    }
}

void unlock(pthread_mutex_t *mutex) {
    if(pthread_mutex_unlock(mutex) == -1) {
        printf("%s", strerror(errno));                
    }
}

int prio_waiting_is_NOT_empty(priosem_t *sem) {
    for(int i = 0; i < sem->threadsCount; i++) {
        if(sem->prio_waiting[i] > 0) {
            return 1;
        }
    }

    return 0;
}

int fetch_highest_prio_waiting(priosem_t *sem) {
    for(int i = sem->threadsCount - 1; i >= 0; i--) {
        if(sem->prio_waiting[i] > 0) {
            return sem->prio_waiting[i];
        }
    }

    return -1;
}

void cond_broadcast(pthread_cond_t *cond, pthread_mutex_t *mutex) {
    if(pthread_cond_broadcast(cond) == -1) {
        printf("%s", strerror(errno));        
    }

    unlock(mutex);
}

void cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
    if(pthread_cond_wait(cond, mutex) == -1) {
        printf("%s", strerror(errno));        
    }
}