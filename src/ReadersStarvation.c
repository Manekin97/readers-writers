#include "ReadersStarvation.h"

void* Reader_r(void* value) {
    while (1) {
        if (pthread_mutex_lock(&m) != 0) {  // Block the mutex (A reader is trying to enter)
            printf("%s", strerror(errno));
        }

        while (!(writers == 0)) {   //  While there are writers inside
            if (pthread_cond_wait(&readersQ, &m) != 0) {    //  Wait
                printf("%s", strerror(errno));
            }
        }

        readers++;  //  Reader entered (Increment number of readers inside)
        readersInQ--;   //  Decrement number of readers in queue
        printf("ReaderQ: %d WriterQ: %d [in: R:%d W:%d]\n", readersInQ,  writersInQ, readers, active_writers);

        if (pthread_mutex_unlock(&m) != 0) {    //  Release the mutex
            printf("%s", strerror(errno));
        }

        sleep(1);   //  Simulate reading

        if (pthread_mutex_lock(&m) != 0) {  //  Block the mutex (A reader is trying to leave)
            printf("%s", strerror(errno));
        }

        if (--readers == 0) {   //  Decrement number of readers inside and if it's the last reader
            if (pthread_cond_signal(&writersQ) != 0) {  //  Signal a writer that library is empty
                printf("%s", strerror(errno));
            }
        }
        readersInQ++;   //  Go back to queue (Increment number of readers in queue)

        printf("ReaderQ: %d  WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readers, active_writers);
        
        if (pthread_mutex_unlock(&m) != 0) {    //  Release the mutex (Reader has left)
            printf("%s", strerror(errno));
        }
    }
}

void* Writer_r(void* value) {
    while (1) {
        if (pthread_mutex_lock(&m) != 0) {  //  Block the mutex (A writer is trying to enter)
            printf("%s", strerror(errno));
        }

        writers++;  //  A writer has entered, but is not writing yet (Increment number of writers inside)
        writersInQ--;   //  Decrement number of writers in queue

        while (!((readers == 0) && (active_writers == 0))) {    //  While there are readers inside or a writer is writing
            if (pthread_cond_wait(&writersQ, &m) != 0) {    //  Wait
                printf("%s", strerror(errno));
            }
        }

        active_writers++;   //  Writer started writing (Increment number of writers writing)
        printf("ReaderQ: %d  WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readers, active_writers);
        
        
        if (pthread_mutex_unlock(&m) != 0) {    //  Release the mutex
            printf("%s", strerror(errno));
        }

        sleep(1);   //  Simulate writing

        if (pthread_mutex_lock(&m) != 0) {  //  Lock the mutex (A writer is trying to leave)
            printf("%s", strerror(errno));
        }

        writers--;  //  Decrement number of writers inside
        active_writers--;   //  Decrement number of writers writing
        writersInQ++;   //  Increment number of writers in queue
        printf("ReaderQ: %d  WriterQ: %d [in: R:%d W:%d]\n", readersInQ, writersInQ, readers, active_writers);      

        if (writers > 0) {  //  If there are more than one writer inside
            if (pthread_cond_signal(&writersQ) != 0) {  //  Signal another writer (Indicate he can enter)
                printf("%s", strerror(errno));
            }
        } else {
            if (pthread_cond_broadcast(&readersQ) != 0) {   //  Signal all readers (Indicate that they can eneter)
                printf("%s", strerror(errno));
            }
        }

        if (pthread_mutex_unlock(&m) != 0) {    //  Release the mutex (A writer has left)
            printf("%s", strerror(errno));
        }
    }
}

void Init_r(int readersCount, int writersCount) {
    readers = 0;
    writers = 0;
    active_writers = 0;
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
        if (pthread_create(&readers[i], NULL, &Reader_r, (void*)ptr)) {
            printf("Error creating reader\n");
        }
    }

    for (int i = 0; i < writersCount; i++) {
        *ptr = i;
        if (pthread_create(&writers[i], NULL, &Writer_r, (void*)ptr)) {
            printf("Error creating writer\n");
        }
    }

    if (pthread_join(readers[0], NULL) == -1) {
        printf("%s", strerror(errno));
    }

    free(ptr);    
}