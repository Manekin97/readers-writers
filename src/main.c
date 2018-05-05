#include "NoStarvation.h"
#include "ReadersStarvation.h"
#include "WritersStarvation.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* const argv[]) {
    const int readersCount = atoi(argv[1]);
    const int writersCount = atoi(argv[2]);

    unsigned int argument;
    while ((argument = getopt(argc, argv, "rwn")) != -1) {
        switch (argument) {
        case 'r':
            Init_r(readersCount, writersCount);
            break;
        case 'w':
            Init(readersCount, writersCount);
            break;
        case 'n':
            Init_o(readersCount, writersCount);
            break;
        case '?':
            printf("Wrong Arguments. \n");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}