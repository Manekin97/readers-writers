#include "ReadersStarvation.h"

void *Reader_r(void *value) {
    int *number = (int*)value;
    
    while(1) {
        //entry
        if (sem_wait(&readTry) == -1) { //  Indicate a reader is trying to enter
            printf("%s", strerror(errno));
        }

        if(pthread_mutex_lock(&rmutex) == -1){  //  lock entry section to avoid race condition with other readers
            printf("%s", strerror(errno));
        }             

        readcount++;    //  report yourself as a reader

        if (readcount == 1) {    //  checks if you are first reader
            if (sem_wait(&resource) == -1) {    //  if you are first reader, lock  the resource
            printf("%s", strerror(errno));
            }   
        }

        if(pthread_mutex_unlock(&rmutex) == -1){    //  release entry section for other readers
            printf("%s", strerror(errno));
        }

        if(sem_post(&readTry) == -1){   //  indicate you are done trying to access the resource
            printf("%s", strerror(errno));
        }


        //critical
        printf("czytelnik %d czyta\n", *number);
        sleep(1);
        printf("czytelnik %d skonczyl czytac\n", *number);

        //exit
        if(pthread_mutex_lock(&rmutex) == -1){  //  reserve exit section - avoids race condition with readers
            printf("%s", strerror(errno));
        }  

        readcount--;        //indicate you're leaving

        if (readcount == 0) {   //  checks if you are last reader leaving
            if(sem_post(&resource) == -1) {   // if last, you must release the locked resource
                printf("%s", strerror(errno));
            }
        }

        if(pthread_mutex_unlock(&rmutex) == -1) {    //  release exit section for other readers
            printf("%s", strerror(errno));
        }    
    }
}

void *Writer_r(void *value) {
    int *number = (int*)value;
    
    while(1) {
        //entry
        if(pthread_mutex_lock(&wmutex) == -1){  //  reserve entry section for writers - avoids race conditions
            printf("%s", strerror(errno));
        }

        writecount++;   //  report yourself as a writer entering

        if (writecount == 1) {  //  checks if you're first writer
            if (sem_wait(&readTry) == -1) { //  if you're first, then you must lock the readers out. Prevent them from trying to enter CS
                printf("%s", strerror(errno));
            }
        }

        if(pthread_mutex_unlock(&wmutex) == -1){    //  release entry section
            printf("%s", strerror(errno));
        }
                    
        //critical
        if (sem_wait(&resource) == -1) {    //  reserve the resource for yourself - prevents other writers from simultaneously editing the shared resource
            printf("%s", strerror(errno));
        }

        printf("%spisarz %d pisze\n", KRED, *number);
        sleep(1);
        printf("%spisarz %d skonczyl pisac\n", KRED, *number);

        if (sem_post(&resource) == -1) {    //  release file
            printf("%s", strerror(errno));
        }

        //exit

        if(pthread_mutex_lock(&wmutex) == -1) { //  reserve exit section
            printf("%s", strerror(errno));
        }

        writecount--;   //  indicate you're leaving

        if (writecount == 0) {  //  checks if you're the last writer
            if (sem_post(&readTry) == -1) { //  if you're last writer, you must unlock the readers. Allows them to try enter CS for reading
                printf("%s", strerror(errno));
            }
        }

        if(pthread_mutex_unlock(&wmutex) == -1) {   //  release exit section
            printf("%s", strerror(errno));
        }
    }
}

void Init_r(int readersCount, int writersCount) {
    readcount = 0;
    writecount = 0;

    pthread_t readers[readersCount];
    pthread_t writers[writersCount];

    if (sem_init(&readTry, 0, 1) == -1) {
        printf("%s", strerror(errno));
    }

    if (sem_init(&resource, 0, 1) == -1) {
        printf("%s", strerror(errno));
    }

    if (pthread_mutex_init(&rmutex, NULL) == -1) {
        printf("%s", strerror(errno));
    }

    if (pthread_mutex_init(&wmutex, NULL) == -1) {
        printf("%s", strerror(errno));
    }

    for (int i = 0; i < readersCount; i++) {
        int *nr = (int*)malloc(sizeof(int));
        *nr = i;
        if (pthread_create(&readers[i], NULL, &Reader_r, (void *)nr)) {
            printf("Error creating reader\n");
        }
    }

    for (int i = 0; i < writersCount; i++) {
        int *nr = (int*)malloc(sizeof(int));
        *nr = i;
        if (pthread_create(&writers[i], NULL, &Writer_r, (void *)nr)) {
            printf("Error creating writer\n");
        }
    }

    if (pthread_join(readers[0], NULL) == -1) {
        printf("%s", strerror(errno));
    }
}