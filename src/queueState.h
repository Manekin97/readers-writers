#ifndef queueState
#define queueState

typedef struct queueState {
    unsigned int readersQ;
    unsigned int writersQ;
    unsigned int readersIn;
    unsigned int writersIn;
} queueState_t;

#endif