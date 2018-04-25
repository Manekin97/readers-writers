#include <stdlib.h> 
#include <stdio.h> 
#include "ReadersStarvation.h"

int main(int argc, char *const argv[]) {
    char *param = argv[3]; 
    readersCount = atoi(argv[1]);
    writersCount = atoi(argv[2]);

    if(strcmp(param, "w") == 0){
        // Init(readersCount, writersCount);
    }

    if(strcmp(param, "r") == 0){
        Init_r(readersCount, writersCount);
    }

}