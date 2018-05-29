#include "NoStarvation.h"

void *Reader_o(void *value) {
    while (1) {
        Wait(&serviceQueue, GetHighestWaitingPriority(&serviceQueue));  //  Wait in line to be serviced

        if (sem_wait(&readCountAccess) == -1) { //  Request exclusive access to readCount
            printf("%s", strerror(errno));
        }

        if (readersInside == 0) { //  if there are no readers already reading
            if (sem_wait(&resourceAccess) == -1) { //   request resource access for readers (writers blocked)
                printf("%s", strerror(errno));
            }
        }

        readersInside++; //  Increase the number of readers inside
        readersInQ--; //  Decrease the number of readers in queue
        printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readersInside, writersInside);

        Post(&serviceQueue); //  let next in line be serviced

        if (sem_post(&readCountAccess) == -1) { //  release access to readCount
            printf("%s", strerror(errno));
        }

        sleep(1); // Simulate reading

        if (sem_wait(&readCountAccess) == -1) { //  request exclusive access to readCount
            printf("%s", strerror(errno));
        }

        readersInside--; //  Decrease the number of readers inside
        readersInQ++; //  Increase the number of readers in queue
        printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readersInside, writersInside);

        if (readersInside == 0) { // if there are no readers left
            if (sem_post(&resourceAccess) == -1) { //  request exclusive access to readCount
                printf("%s", strerror(errno));
            }
        }

        if (sem_post(&readCountAccess) == -1) { //  release access to readCount
            printf("%s", strerror(errno));
        }
    }
}

void *Writer_o(void *value) {
    while (1) {
        Wait(&serviceQueue, GetHighestWaitingPriority(&serviceQueue)); //  wait in line to be serviced

        if (sem_wait(&resourceAccess) == -1) { //  request exclusive access to resource
            printf("%s", strerror(errno));
        }

        Post(&serviceQueue); //  let next in line be serviced

        writersInside++; //  Increase the number of writers inside
        writersInQ--; //  Decrease the number of writers in queue
        printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readersInside, writersInside);

        sleep(1);

        writersInside--; //  Decrease the number of writers inside
       writersInQ++; //  Increase the number of writers in queue
        printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readersInside, writersInside);

        if (sem_post(&resourceAccess) == -1) { // release resource access for next reader/writer
            printf("%s", strerror(errno));
        }
    }
}

void Init_o(int readersCount, int writersCount){
    /* Initialize queue status */
    readersInside = 0;
    writersInside = 0;
    readersInQ = readersCount;
    writersInQ = writersCount;

    /* Initialize readers and writers set */
    pthread_t readers[readersCount];
    pthread_t writers[writersCount];

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
        printf("%s", strerror(errno));
    }

    if (pthread_cond_init(&(serviceQueue.cv), NULL) == -1) {
        printf("%s", strerror(errno));
    }

    if (sem_init(&resourceAccess, 0, 1) == -1) {
        printf("%s", strerror(errno));
    }

    if (sem_init(&readCountAccess, 0, 1) == -1) {
        printf("%s", strerror(errno));
    }

    /* Create threads */
    int* ptr = (int*)(malloc(sizeof(int)));
    for (int i = 0; i < readersCount; i++) {
        *ptr = i;
        if (pthread_create(&readers[i], NULL, &Reader_o, (void*)ptr)) {
            printf("Error creating reader\n");
        }
    }

    for (int i = 0; i < writersCount; i++) {
        *ptr = i;
        if (pthread_create(&writers[i], NULL, &Writer_o, (void*)ptr)) {
            printf("Error creating writer\n");
        }
    }

    if (pthread_join(readers[0], NULL) == -1) {
        printf("%s", strerror(errno));
    }

    free(serviceQueue.prio_waiting);
    free(serviceQueue.prio_released);
    free(ptr);
}