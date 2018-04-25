#include "ReadersStarvation.h"

void *Reader_r(void *value) {
    int *number = (int*)value;
    
    while(1){
        if(sem_wait(&restReadersSem) == -1){
            printf("%s", strerror(errno));
        }

        if(sem_wait(&readersSem) == -1){
            printf("%s", strerror(errno));
        }

        if(sem_wait(&rpeekSem) == -1){
            printf("%s", strerror(errno));
        }

        readersInside++;

        if(readersInside == 1){
            if(pthread_mutex_lock(&writersSem) == -1){
                printf("%s", strerror(errno));
            }
        }

        if(sem_post(&rpeekSem) == -1){
            printf("%s", strerror(errno));
        }

        if(sem_post(&readersSem) == -1){
            printf("%s", strerror(errno));
        }

        if(sem_post(&restReadersSem) == -1){
            printf("%s", strerror(errno));
        }

        printf("czytelnik %d czyta\n", *number);
        sleep(1);
        printf("czytelnik %d skonczyl czytac\n", *number);

        if(sem_wait(&rpeekSem) == -1){
            printf("%s", strerror(errno));
        }

        readersInside--;

        if(readersInside == 0) {
            if(pthread_mutex_unlock(&writersSem) == -1){
                printf("%s", strerror(errno));
            }
        }

        if(sem_post(&rpeekSem) == -1){
            printf("%s", strerror(errno));
        }
    }
}

void *Writer_r(void *value) {
    int *number = (int*)value;

    while(1) {
        if(sem_wait(&wpeekSem) == -1){
            printf("%s", strerror(errno));
        }

        writersInside++;

        if(writersCount == 1){
            if(sem_wait(&readersSem) == -1){
                printf("%s", strerror(errno));
            }
        }

        if(sem_post(&wpeekSem) == -1){
            printf("%s", strerror(errno));
        }

        if(pthread_mutex_lock(&writersSem) == -1){
            printf("%s", strerror(errno));
        }

        printf("%spisarz %d pisze\n", KRED, *number);
        sleep(1);
        printf("%spisarz %d skonczyl pisac\n", KRED, *number); 

        if(pthread_mutex_unlock(&writersSem) == -1){
            printf("%s", strerror(errno));
        }
        
        if(sem_wait(&wpeekSem) == -1){
            printf("%s", strerror(errno));
        }

        writersInside--;
        
        if(writersCount == 0) {
            if(sem_post(&readersSem) == -1){
                printf("%s", strerror(errno));
            }
        }

        if(sem_post(&wpeekSem) == -1){
            printf("%s", strerror(errno));
        }
    }
}

void Init_r(int readersCount, int writersCount){
    writersInside = 0;
    readersInside = 0;

    pthread_t readers[readersCount];
    pthread_t writers[writersCount];
    
    if(sem_init(&readersSem, 0, readersCount) == -1){
        printf("%s", strerror(errno));
    }

    if(sem_init(&restReadersSem, 0, readersCount - 1) == -1){
        printf("%s", strerror(errno));
    }

    if(sem_init(&rpeekSem, 0, readersCount) == -1){
        printf("%s", strerror(errno));
    }

    if(sem_init(&wpeekSem, 0, writersCount) == -1){
        printf("%s", strerror(errno));
    }

    if(pthread_mutex_init(&writersSem, NULL) == -1){
        printf("%s", strerror(errno));
    }

    for(int i = 0; i < readersCount; i++) {
        int *nr = malloc(sizeof(int));
        *nr = i;
        if (pthread_create(&readers[i], NULL, &Reader_r, (void*)nr)){
            printf("Error creating reader\n");
        }
    }

    for(int i = 0; i < writersCount; i++) {
        int *nr = malloc(sizeof(int));
        *nr = i;    
        if (pthread_create(&writers[i], NULL, &Writer_r, (void*)nr)){
            printf("Error creating writer\n");
        }
    }

    if(pthread_join(readers[0], NULL) == -1){
        printf("%s", strerror(errno));
    }
}

