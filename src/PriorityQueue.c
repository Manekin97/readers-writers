#include "PriorityQueue.h"

int Post(priosem_t *sem) {
	if (Lock(&(sem->mutex)) == -1) {	//	Lock the mutex
		return -1;
	}

	sem->value++;

	if (sem->value <= 0 && IsThreadWaiting(sem)) {	//	If there are threads waiting
		int prio = GetHighestWaitingPriority(sem);	//	Get the highest priority

		sem->prio_waiting[prio]--;	//	Decrement the number of waiting threads with priority prio
		sem->prio_released[prio]++;	//	Increment the number of released threads with priority prio

		if (Cond_broadcast(&(sem->cv), &(sem->mutex)) == 1) {	//	Signal other threads
			return -1;
		}
	}

	if (Unlock(&(sem->mutex)) == -1) {	//	Unlock the mutex
		return -1;
	}
}

int Wait(priosem_t *sem, int prio) {
	if (Lock(&(sem->mutex)) == -1) {	//	Lock the mutex
		return -1;
	}

	sem->value--;

	if (sem->value < 0) {	//	If there are threads waiting
		sem->prio_waiting[prio]++;	// Get in line (Increment the number of waiting threads with priority prio)
		while (sem->prio_released[prio] < 0) {
			if (Cond_wait(&(sem->cv), &(sem->mutex)) == -1) {
				return -1;
			}
		}

		// ok to leave
		sem->prio_released[prio]--;
	}

	if (Unlock(&(sem->mutex)) == -1) {
		return -1;
	}
}

int Lock(pthread_mutex_t *mutex) {
	if (pthread_mutex_lock(mutex) == -1) {
		return -1;
	}

	return 0;
}

int Unlock(pthread_mutex_t *mutex) {
	if (pthread_mutex_unlock(mutex) == -1) {
		return -1;
	}

	return 0;
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

int Cond_broadcast(pthread_cond_t *cond, pthread_mutex_t *mutex) {
	if (pthread_cond_broadcast(cond) == -1) {
		return -1;
	}

	if (Unlock(mutex) == -1) {
		return -1;
	}

	return 0;
}

int Cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
	if (pthread_cond_wait(cond, mutex) == -1) {
		return -1;
	}

	return 0;
}