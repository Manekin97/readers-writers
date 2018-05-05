#ifndef queueState
#define queueState

typedef struct queueState {
    unsigned int readersQ;
    unsigned int writersQ;
} queueState_t;

#endif