#include "NoStarvation.h"

void *Reader_o(void *value) {
    int *number = (int*)value;
    
    while(1){
        wait(&serviceQueue, fetch_highest_prio_waiting(&serviceQueue)); //  wait in line to be serviced

        if(sem_wait(&readCountAccess) == -1){   //  request exclusive access to readCount
            printf("%s", strerror(errno));
        }

        //enter
        if (readCount == 0) {   //  if there are no readers already reading
            if(sem_wait(&resourceAccess) == -1){   //   request resource access for readers (writers blocked)
                printf("%s", strerror(errno));
            }
        }

        readCount++;    //  update count of active readers

        post(&serviceQueue);    //  let next in line be serviced

        if(sem_post(&readCountAccess) == -1){   //  release access to readCount
            printf("%s", strerror(errno));
        }

        //read
        printf("%sczytelnik %d czyta\n", KGRN, *number);
        sleep(1);
        printf("%sczytelnik %d skonczyl czytac\n", KGRN, *number); 

        if(sem_wait(&readCountAccess) == -1){   //  request exclusive access to readCount
            printf("%s", strerror(errno));
        }

        //exit
        readCount--;            // update count of active readers

        if (readCount == 0) { // if there are no readers left
            if(sem_post(&resourceAccess) == -1){   //  request exclusive access to readCount
                printf("%s", strerror(errno));
            }
        }

        if(sem_post(&readCountAccess) == -1){   //  release access to readCount
            printf("%s", strerror(errno));
        }
    }
}

void *Writer_o(void *value) {
    int *number= (int*)value;

    while(1) {
        wait(&serviceQueue, fetch_highest_prio_waiting(&serviceQueue)); //  wait in line to be serviced        
        
        //enter
        if(sem_wait(&resourceAccess) == -1){  //  request exclusive access to resource
            printf("%s", strerror(errno));
        }

        post(&serviceQueue);    //  let next in line be serviced      
    
        //write
        printf("%spisarz %d pisze\n", KRED, *number);
        sleep(1);
        printf("%spisarz %d skonczyl pisac\n", KRED, *number); 
    
        //exit
        if(sem_post(&resourceAccess) == -1){  // release resource access for next reader/writer
            printf("%s", strerror(errno));
        }
    }
}

void Init_o(int readersCount, int writersCount){
    readCount = 0;

    pthread_t readers[readersCount];
    pthread_t writers[writersCount];

    serviceQueue.value = 1;
    serviceQueue.threadsCount = readersCount + writersCount;
    serviceQueue.prio_waiting = malloc(serviceQueue.threadsCount * sizeof(int));
    serviceQueue.prio_released = malloc(serviceQueue.threadsCount * sizeof(int));

    serviceQueue.prio_waiting[0] = serviceQueue.threadsCount;
    serviceQueue.prio_released[0] = 0;

    for(int i = 1; i < serviceQueue.threadsCount; i++) {
        serviceQueue.prio_waiting[i] = 0;
        serviceQueue.prio_released[i] = 0;
    }

    if(pthread_mutex_init(&(serviceQueue.mutex), NULL) == -1) {
        printf("%s", strerror(errno));
    }

    if(pthread_cond_init(&(serviceQueue.cv), NULL) == -1) {
        printf("%s", strerror(errno));
    }
    
    if(sem_init(&resourceAccess, 0, 1) == -1) {
        printf("%s", strerror(errno));
    }

    if(sem_init(&readCountAccess, 0, 1) == -1) {
        printf("%s", strerror(errno));
    }

    for(int i = 0; i < readersCount; i++) {
        int *nr = (int*)malloc(sizeof(int));
        *nr = i;
        if (pthread_create(&readers[i], NULL, &Reader_o, (void*)nr)){
            printf("Error creating reader\n");
        }
    }

    for(int i = 0; i < writersCount; i++) {
        int *nr = (int*)malloc(sizeof(int));
        *nr = i;    
        if (pthread_create(&writers[i], NULL, &Writer_o, (void*)nr)){
            printf("Error creating writer\n");
        }
    }

    if(pthread_join(readers[0], NULL) == -1){
        printf("%s", strerror(errno));
    }
}