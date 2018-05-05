#include "PriorityQueue.h"

void Post(priosem_t *sem){
    Lock(&(sem->mutex));
    sem->value++;

    if (sem->value <= 0 && IsThreadWaiting(sem)) {
        int prio = GetHighestWaitingPriority(sem);

        sem->prio_waiting[prio]--;
        sem->prio_released[prio]++;

        Cond_broadcast(&(sem->cv), &(sem->mutex));
    }

    Unlock(&(sem->mutex));
}

void Wait(priosem_t *sem, int prio) {
    Lock(&(sem->mutex));
    sem->value--;

    if (sem->value < 0) {
        // get in line
        sem->prio_waiting[prio]++;
        while (sem->prio_released[prio] < 0) {
            Cond_wait(&(sem->cv), &(sem->mutex));
        }

        // ok to leave
        sem->prio_released[prio]--;
    }

    Unlock(&(sem->mutex));
}

void Lock(pthread_mutex_t *mutex) {
    if (pthread_mutex_lock(mutex) == -1) {
        printf("%s", strerror(errno));
    }
}

void Unlock(pthread_mutex_t *mutex) {
    if (pthread_mutex_unlock(mutex) == -1) {
        printf("%s", strerror(errno));
    }
}

int IsThreadWaiting(priosem_t *sem) {
    for (int i = 0; i < sem->threadsCount; i++) {
        if (sem->prio_waiting[i] > 0) {
            return 1;
        }
    }

    return 0;
}

int GetHighestWaitingPriority(priosem_t *sem) {
    for (int i = sem->threadsCount - 1; i >= 0; i--) {
        if (sem->prio_waiting[i] > 0) {
            return sem->prio_waiting[i];
        }
    }

    return -1;
}

void Cond_broadcast(pthread_cond_t *cond, pthread_mutex_t *mutex) {
    if (pthread_cond_broadcast(cond) == -1) {
        printf("%s", strerror(errno));
    }

    Unlock(mutex);
}

void Cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
    if (pthread_cond_wait(cond, mutex) == -1) {
        printf("%s", strerror(errno));
    }
}