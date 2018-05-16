#include "ReadersStarvation.h"

void* Reader_r(void* value) {
    while (1) {
        if (pthread_mutex_lock(&m) != 0) {
            printf("%s", strerror(errno));
        }

        while (!(writers == 0)) {
            if (pthread_cond_wait(&readersQ, &m) != 0) {
                printf("%s", strerror(errno));
            }
        }

        readers++;
        printf("[in: R:%d W:%d]\n", readers, active_writers);

        if (pthread_mutex_unlock(&m) != 0) {
            printf("%s", strerror(errno));
        }

        sleep(1);

        if (pthread_mutex_lock(&m) != 0) {
            printf("%s", strerror(errno));
        }

        if (--readers == 0) {
            if (pthread_cond_signal(&writersQ) != 0) {
                printf("%s", strerror(errno));
            }
        }

        printf("[in: R:%d W:%d]\n", readers, active_writers);

        if (pthread_mutex_unlock(&m) != 0) {
            printf("%s", strerror(errno));
        }
    }
}

void* Writer_r(void* value) {
    while (1) {
        if (pthread_mutex_lock(&m) != 0) {
            printf("%s", strerror(errno));
        }

        writers++;

        while (!((readers == 0) && (active_writers == 0))) {
            if (pthread_cond_wait(&writersQ, &m) != 0) {
                printf("%s", strerror(errno));
            }
        }

        active_writers++;
        printf("[in: R:%d W:%d]\n", readers, active_writers);

        if (pthread_mutex_unlock(&m) != 0) {
            printf("%s", strerror(errno));
        }

        sleep(1);

        if (pthread_mutex_lock(&m) != 0) {
            printf("%s", strerror(errno));
        }

        writers--;
        active_writers--;
        printf("[in: R:%d W:%d]\n", readers, active_writers);

        if (writers > 0) {
            if (pthread_cond_signal(&writersQ) != 0) {
                printf("%s", strerror(errno));
            }
        } else {
            if (pthread_cond_broadcast(&readersQ) != 0) {
                printf("%s", strerror(errno));
            }
        }

        if (pthread_mutex_unlock(&m) != 0) {
            printf("%s", strerror(errno));
        }
    }
}

void Init_r(int readersCount, int writersCount) {
    readers = 0;
    writers = 0;
    active_writers = 0;

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
}