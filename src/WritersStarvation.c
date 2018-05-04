#include "WritersStarvation.h"

void *Reader(void *value) {
    int *number = (int*)value;
    
    while(1){
        if(sem_wait(&readersSem) == -1){
            printf("%s", strerror(errno));
        }

        readersInside++;

        if(readersInside == 1){
            if(pthread_mutex_lock(&writersSem) == -1){
                printf("%s", strerror(errno));
            }
        }

        if(sem_post(&readersSem) == -1){
            printf("%s", strerror(errno));
        }

        printf("czytelnik %d czyta\n", *number);
        sleep(1);
        printf("czytelnik %d skonczyl czytac\n", *number);        
        
        if(sem_wait(&readersSem) == -1){
            printf("%s", strerror(errno));            
        }

        readersInside--;

        if(readersInside == 0){
            if(pthread_mutex_unlock(&writersSem) == -1){
                printf("%s", strerror(errno));                
            }
        }

        if(sem_post(&readersSem) == -1){
            printf("%s", strerror(errno));
        }
    }
}

void *Writer(void *value) {
    int *number= (int*)value;

    while(1) {
        if(pthread_mutex_lock(&writersSem) == -1){
            printf("%s", strerror(errno));            
        }

        printf("%spisarz %d pisze\n", KRED, *number);
        sleep(1);
        printf("%spisarz %d skonczyl pisac\n", KRED, *number);  

        if(pthread_mutex_unlock(&writersSem)== -1){
            printf("%s", strerror(errno));           
        }
    }
}

void Init(int readersCount, int writersCount){
    readersInside = 0;

    pthread_t readers[readersCount];
    pthread_t writers[writersCount];
    
    if(sem_init(&readersSem, 0, 1) == -1){
        printf("%s", strerror(errno));
    }

    if(pthread_mutex_init(&writersSem, NULL) == -1){
        printf("%s", strerror(errno));
    }

    for(int i = 0; i < readersCount; i++) {
        int *nr = (int*)malloc(sizeof(int));
        *nr = i;
        if (pthread_create(&readers[i], NULL, &Reader, (void*)nr)){
            printf("Error creating reader\n");
        }
    }

    for(int i = 0; i < writersCount; i++) {
        int *nr = (int*)malloc(sizeof(int));
        *nr = i;    
        if (pthread_create(&writers[i], NULL, &Writer, (void*)nr)){
            printf("Error creating writer\n");
        }
    }

    if(pthread_join(readers[0], NULL) == -1){
        printf("%s", strerror(errno));
    }
}

