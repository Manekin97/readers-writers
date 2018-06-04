#include "WritersStarvation.h"

void* Reader(void* value) {
	while (1) {
		if (pthread_mutex_lock(&m) != 0) {  //  Block the mutex (A reader is trying to enter)
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		while (!(writers == 0)) {   //  While there are writers inside
			if (pthread_cond_wait(&readersQ, &m) != 0) {    //  Wait
				fprintf(stderr, "%s \n", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}

		readers++;  //  Reader entered (Increment number of readers inside)
		readersInQ--;   //  Decrement number of readers in queue
		printf("ReaderQ: %d  WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readers, writers);

		if (pthread_mutex_unlock(&m) != 0) {    //  Release the mutex    
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		sleep(1);   //  Simulate reading

		if (pthread_mutex_lock(&m) != 0) {  //  Block the mutex (A reader is trying to leave)
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (--readers == 0) {    //  Decrement number of readers inside and if it's the last reader
			if (pthread_cond_signal(&writersQ) != 0) {  //  Signal a writer that library is empty
				fprintf(stderr, "%s \n", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}

		readersInQ++;   //  Go back to queue (Increment number of readers in queue)
		printf("ReaderQ: %d  WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readers, writers);

		if (pthread_mutex_unlock(&m) != 0) {    //  Release the mutex (A Reader has left)
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
}

void* Writer(void* value) {
	while (1) {
		if (pthread_mutex_lock(&m) != 0) {  //  Block the mutex (A writer is trying to enter)
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		while (!((readers == 0) && (writers == 0))) {   //  While there are readers or writers inside 
			if (pthread_cond_wait(&writersQ, &m) != 0) {
				fprintf(stderr, "%s \n", strerror(errno));
				exit(EXIT_FAILURE);
			}
		}

		writers++;  //  Increment number of writers inside
		writersInQ--;   //  Decrement number of writers in queue
		printf("ReaderQ: %d  WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readers, writers);

		if (pthread_mutex_unlock(&m) != 0) {    //  Release the mutex
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		sleep(1);   //  Simulate writing

		if (pthread_mutex_lock(&m) != 0) {  //  Lock the mutex (A writer is trying to leave)
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		writers--;  //  Decrement number of writers inside
		writersInQ++;   //  Increment number of writers in queue
		printf("ReaderQ: %d  WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readers, writers);

		if (pthread_cond_broadcast(&readersQ) != 0) {   //  Signal all readers that they can enter
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		
		if (pthread_cond_signal(&writersQ) != 0) {  //  Signal a writer that he can enter
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (pthread_mutex_unlock(&m) != 0) {    //  Release the mutex (A writer has left)
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
}

void Init(int readersCount, int writersCount) {
	readers = 0;
	writers = 0;
	readersInQ = readersCount;
	writersInQ = writersCount;

	/* Initialize readers and writers set */
	pthread_t *readersArray = (pthread_t*)malloc(sizeof(int) * readersCount);
	pthread_t *writersArray = (pthread_t*)malloc(sizeof(int) * writersCount);

	if (pthread_cond_init(&readersQ, NULL) != 0) {
		fprintf(stderr, "%s \n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (pthread_cond_init(&writersQ, NULL) != 0) {
		fprintf(stderr, "%s \n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (pthread_mutex_init(&m, NULL) == -1) {
		fprintf(stderr, "%s \n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Create threads */
	int* ptr = (int*)(malloc(sizeof(int)));
	for (int i = 0; i < readersCount; i++) {
		*ptr = i;
		if (pthread_create(&readersArray[i], NULL, &Reader, (void*)ptr)) {
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < writersCount; i++) {
		*ptr = i;
		if (pthread_create(&writersArray[i], NULL, &Writer, (void*)ptr)) {
			fprintf(stderr, "%s \n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	if (pthread_join(readersArray[0], NULL) == -1) {
		fprintf(stderr, "%s \n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	free(ptr);
	free(readersArray);
	free(writersArray);
}