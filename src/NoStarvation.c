#include "NoStarvation.h"

void *Reader_o(void *value) {
	while (1) {
		if (Wait(&serviceQueue, GetHighestWaitingPriority(&serviceQueue)) == -1) {  //  Wait in line to be serviced
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		};

		if (sem_wait(&readersAccess) == -1) {	//	Lock the semaphore to ensure only one thread has acces to readers variable
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (readers == 0) {	//	If there are no readers inside
			if (sem_wait(&libraryAccess) == -1) {	//	Lock the semaphore (Prevent writers from accessing the library)
				fprintf(stderr, "%s \n", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}

		readers++;	//	Increase the number of readers inside
		readersInQ--;	//	Decrease the number of readers in queue
		printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readers, writers);

		if (Post(&serviceQueue) == -1) {	//	Let next in line to be serviced
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (sem_post(&readersAccess) == -1) {	//	Release the semaphore
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		sleep(1);	//	Simulate reading

		if (sem_wait(&readersAccess) == -1) {	//	Lock the semaphore to ensure only one thread has acces to readers variable
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		readers--;	//	Decrease the number of readers inside
		readersInQ++;	//	Increase the number of readers in queue
		printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readers, writers);

		if (readers == 0) {	//	If it's the last reader
			if (sem_post(&libraryAccess) == -1) {	//	Unlock the semaphore (Allow writers to access the library)
				fprintf(stderr, "%s \n", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}

		if (sem_post(&readersAccess) == -1) {	//	Release the semaphore
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
}

void *Writer_o(void *value) {
	while (1) {
		if (Wait(&serviceQueue, GetHighestWaitingPriority(&serviceQueue)) == -1) {	//	Wait in line to be serviced
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		};

		if (sem_wait(&libraryAccess) == -1) {	//	Lock the semaphore (Prevent readers or other writers from accessing the library)
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (Post(&serviceQueue) == -1) {	//	Let next in line be serviced
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		writers++;	//	Increase the number of writers inside
		writersInQ--;	//	Decrease the number of writers in queue
		printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readers, writers);

		sleep(1);

		writers--;	//	Decrease the number of writers inside
		writersInQ++;	//	Increase the number of writers in queue
		printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readers, writers);

		if (sem_post(&libraryAccess) == -1) {	//	Unlock the semaphore to allow reader/writer to acces the library
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
}

void Init_o(int readersCount, int writersCount) {
	/* Initialize queue status */
	readers = 0;
	writers = 0;
	readersInQ = readersCount;
	writersInQ = writersCount;

	/* Initialize readers and writers set */
	pthread_t *readersArray = (int*)malloc(sizeof(int) * readersCount);
	pthread_t *writersArray = (int*)malloc(sizeof(int) * writersCount);

	serviceQueue.value = 1;
	serviceQueue.threadsCount = readersCount + writersCount;
	serviceQueue.prio_waiting = malloc(serviceQueue.threadsCount * sizeof(int));
	serviceQueue.prio_released = malloc(serviceQueue.threadsCount * sizeof(int));

	serviceQueue.prio_waiting[0] = serviceQueue.threadsCount;
	serviceQueue.prio_released[0] = 0;

	for (int i = 1; i < serviceQueue.threadsCount; i++) {
		serviceQueue.prio_waiting[i] = 0;
		serviceQueue.prio_released[i] = 0;
	}

	if (pthread_mutex_init(&(serviceQueue.mutex), NULL) == -1) {
		fprintf(stderr, "%s \n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (pthread_cond_init(&(serviceQueue.cv), NULL) == -1) {
		fprintf(stderr, "%s \n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (sem_init(&libraryAccess, 0, 1) == -1) {
		fprintf(stderr, "%s \n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (sem_init(&readersAccess, 0, 1) == -1) {
		fprintf(stderr, "%s \n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Create threads */
	int* ptr = (int*)(malloc(sizeof(int)));
	for (int i = 0; i < readersCount; i++) {
		*ptr = i;
		if (pthread_create(&readersArray[i], NULL, &Reader_o, (void*)ptr)) {
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < writersCount; i++) {
		*ptr = i;
		if (pthread_create(&writersArray[i], NULL, &Writer_o, (void*)ptr)) {
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	if (pthread_join(readersArray[0], NULL) == -1) {
		fprintf(stderr, "%s \n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	free(serviceQueue.prio_waiting);
	free(serviceQueue.prio_released);
	free(ptr);
	free(readersArray);
	free(writersArray);
}