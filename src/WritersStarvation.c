#include "WritersStarvation.h"

void* Reader(void* value) {
    while (1) {
        if (pthread_mutex_lock(&m) != 0) {  //  Block the mutex (A reader is trying to enter)
            printf("%s", strerror(errno));
        }

        while (!(writers == 0)) {   //  While there are writers inside
            if (pthread_cond_wait(&readersQ, &m) != 0) {    //  Wait
                printf("%s", strerror(errno));
            }
        }

        readers++;  //  Reader entered (Increment number of readers inside)
        readersInQ--;   //  Decrement number of readers in queue
        printf("ReaderQ: %d  WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readers, writers);

        if (pthread_mutex_unlock(&m) != 0) {    //  Release the mutex    
            printf("%s", strerror(errno));
        }

        sleep(1);   //  Simulate reading

        if (pthread_mutex_lock(&m) != 0) {  //  Block the mutex (A reader is trying to leave)
            printf("%s", strerror(errno));
        }

        if (--readers == 0) {    //  Decrement number of readers inside and if it's the last reader
            if (pthread_cond_signal(&writersQ) != 0) {  //  Signal a writer that library is empty
                printf("%s", strerror(errno));
            }
        }

        readersInQ++;   //  Go back to queue (Increment number of readers in queue)
        printf("ReaderQ: %d  WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readers, writers);

        if (pthread_mutex_unlock(&m) != 0) {    //  Release the mutex (A Reader has left)
            printf("%s", strerror(errno));
        }
    }
}

void* Writer(void* value) {
    while (1) {
        if (pthread_mutex_lock(&m) != 0) {  //  Block the mutex (A writer is trying to enter)
            printf("%s", strerror(errno));
        }

        while (!((readers == 0) && (writers == 0))) {   //  While there are readers inside or a writers
            if (pthread_cond_wait(&writersQ, &m) != 0) {
                printf("%s", strerror(errno));
            }
        }

        writers++;  //  Increment number of writers inside
        writersInQ--;   //  Decrement number of writers in queue
        printf("ReaderQ: %d  WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readers, writers);     

        if (pthread_mutex_unlock(&m) != 0) {    //  Release the mutex
            printf("%s", strerror(errno));
        }

        sleep(1);   //  Simulate writing

        if (pthread_mutex_lock(&m) != 0) {  //  Lock the mutex (A writer is trying to leave)
            printf("%s", strerror(errno));
        }

        writers--;  //  Decrement number of writers inside
        writersInQ++;   //  Increment number of writers in queue
        printf("ReaderQ: %d  WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readers, writers);      

        if (pthread_cond_signal(&writersQ) != 0) {  //  Signal a writer that he can enter
            printf("%s", strerror(errno));
        }

        if (pthread_cond_broadcast(&readersQ) != 0) {   //  Signal all readers that they can enter
            printf("%s", strerror(errno));
        }

        if (pthread_mutex_unlock(&m) != 0) {    //  Release the mutex (A writer has left)
            printf("%s", strerror(errno));
        }
    }
}

void Init(int readersCount, int writersCount) {;
    readers = 0;
    writers = 0;
    readersInQ = readersCount;
    writersInQ = writersCount;

    /* Initialize readers and writers set */
    pthread_t readers[readersCount];
    pthread_t writers[writersCount];

    if (pthread_cond_init(&readersQ, NULL) != 0) {
        printf("%s", strerror(errno));
    }

    if (pthread_cond_init(&writersQ, NULL) != 0) {
        printf("%s", strerror(errno));
    }

    if (pthread_mutex_init(&m, NULL) == -1) {
        printf("%s", strerror(errno));
    }

    /* Create threads */
    int* ptr = (int*)(malloc(sizeof(int)));
    for (int i = 0; i < readersCount; i++) {
        *ptr = i;
        if (pthread_create(&readers[i], NULL, &Reader, (void*)ptr)) {
            printf("Error creating reader\n");
        }
    }

    for (int i = 0; i < writersCount; i++) {
        *ptr = i;
        if (pthread_create(&writers[i], NULL, &Writer, (void*)ptr)) {
            printf("Error creating writer\n");
        }
    }

    if (pthread_join(readers[0], NULL) == -1) {
        printf("%s", strerror(errno));
    }

    free(ptr);
}