#include <stdio.h>
#include <stdlib.h>

#define FIFO 0
#define LRU 1

char *trace_path = NULL;
int cache_size = -1, block_size = -1, associativity = -1, replacement = -1;

int main(int argc, char *argv[]) {
	while(argc > 0) {
		if(argv[0][0] != '-') {
			exit(2);
		}
		switch(argv[0][1]) {
		case 't':
			trace_path = argv[1];
			break;
		case 's':
			cache_size = atoi(argv[1]);
			break;
		case 'l':
			block_size = atoi(argv[1]);
			break;
		case 'a':
			associativity = atoi(argv[1]);
			if(associativity != 0 || associativity != 1) {
				exit(2);
			}
			break;
		case 'r':
			replacement = atoi(argv[1]);
			if(replacement != FIFO || replacement != LRU) {
				exit(2);
			}
			break;
		default:
			printf("Unknown option %s.\n", argv[0]);
			exit(2);
		}

		argc -= 2;
		argv += 2;
	}
	if(trace_path == NULL || cache_size == -1 || block_size == -1 || associativity == -1 || replacement == -1) {
		puts("Miss arguments.");
		exit(2);
	}
}
