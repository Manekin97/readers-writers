#include "WritersStarvation.h"

void* Reader(void* value) {
    queueState_t* qstate = (queueState_t*)value;

    while (1) {
        if (sem_wait(&readTry) == -1) { //  Indicate a reader is trying to enter
            printf("%s", strerror(errno));
        }

        readersInside++; //  Increase the number of readers inside
        qstate->readersQ--; //  Decrease the number of readers in queue
        printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n", qstate->readersQ, qstate->writersQ, readersInside, writersInside);        

        if (readersInside == 1) { //  if it's the first reader
            if (pthread_mutex_lock(&wmutex) == -1) { //  Lock writers
                printf("%s", strerror(errno));
            }
        }

        if (sem_post(&readTry) == -1) { //  Indicate a reader is done trying to enter
            printf("%s", strerror(errno));
        }

        sleep(1); //  Simulate reading

        if (sem_wait(&readTry) == -1) { //  Indicate a reader is trying to leave
            printf("%s", strerror(errno));
        }

        readersInside--; //  Decrease the number of readers in inside
        qstate->readersQ++; //  Increase the number of readers queue
        printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n", qstate->readersQ, qstate->writersQ, readersInside, writersInside);        

        if (readersInside == 0) { //  if it's the last reader
            if (pthread_mutex_unlock(&wmutex) == -1) { //  Unlock writers
                printf("%s", strerror(errno));
            }
        }

        if (sem_post(&readTry) == -1) { //  Indicate a reader is done trying to leave
            printf("%s", strerror(errno));
        }
    }
}

void* Writer(void* value) {
    queueState_t* qstate = (queueState_t*)value;

    while (1) {
        if (pthread_mutex_lock(&wmutex) == -1) { // Writer is trying to enter
            printf("%s", strerror(errno));
        }

        writersInside++; //  Increase the number of writers inside
        qstate->writersQ--; //  Decrease the number of writers in queue
        printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n", qstate->readersQ, qstate->writersQ, readersInside, writersInside);

        sleep(1); //  Simulate writing

        if (pthread_mutex_unlock(&wmutex) == -1) { // Writer is trying to leave
            printf("%s", strerror(errno));
        }

        writersInside--; //  Decrease the number of writers inside
        qstate->writersQ++; //  Increase the number of writers in queue
        printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n", qstate->readersQ, qstate->writersQ, readersInside, writersInside);
    }
}

void Init(int readersCount, int writersCount) {
    /* Initialize queue status */
    state = malloc(sizeof(queueState_t));
    state->readersQ = readersCount;
    state->writersQ = writersCount;

    readersInside = 0;
    writersInside = 0;

    /* Initialize readers and writers set */
    pthread_t readers[readersCount];
    pthread_t writers[writersCount];

    if (sem_init(&readTry, 0, 1) == -1) {
        printf("%s", strerror(errno));
    }

    if (pthread_mutex_init(&wmutex, NULL) == -1) {
        printf("%s", strerror(errno));
    }

    /* Create threads */
    for (int i = 0; i < readersCount; i++) {
        if (pthread_create(&readers[i], NULL, &Reader, (void*)state)) {
            printf("Error creating reader\n");
        }
    }

    for (int i = 0; i < writersCount; i++) {
        if (pthread_create(&writers[i], NULL, &Writer, (void*)state)) {
            printf("Error creating writer\n");
        }
    }

    if (pthread_join(readers[0], NULL) == -1) {
        printf("%s", strerror(errno));
    }
}