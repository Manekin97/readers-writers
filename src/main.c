#include "NoStarvation.h"
#include "ReadersStarvation.h"
#include "WritersStarvation.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* const argv[]) {
	if (argc < 4) {
		fprintf(stderr, "Wrong number of arguments. \n");
		exit(EXIT_FAILURE);
	}

	const int readersCount = atoi(argv[1]);
	const int writersCount = atoi(argv[2]);

	if (readersCount <= 0 || writersCount <= 0) {
		fprintf(stderr, "Number of readers and writers must be a positive integer. \n");
		exit(EXIT_FAILURE);
	}

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
			printf("Available options: \n");
			printf("    -r (readers starvation) \n");
			printf("    -w (writers starvation) \n");
			printf("    -r (fair solution) \n");

			exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}