#include <stdio.h>
#include <stdlib.h>

#define FIFO 0
#define LRU 1

char *trace_path = NULL;
int cache_size = -1, block_size = -1, associativity = -1, replacement = -1;

void print_usage(const char *prog_name) {
	printf(
		"Usage: %s -t trace_file -s cache_size -l block_size -a associativity -r replacement\n \
		arguments:\n \
			trace_file: The path to the input file which contains those memory address to access.\n \
			cache_size: The size of the cache. 1~1024 (KByte).\n \
			block_size: The size of a cache block. 4~256 (Byte).\n \
			associativity: The number of the way.\n \
			replacement: 0 for FIFO and 1 for LRU.\n",
		prog_name
	);

	exit(2);
}

void arg_fetch(int argc, char *argv[]) {
	const char *prog_name = argv[0];
	++argv, --argc;

	while(argc > 0) {
		if(argv[0][0] != '-') {
			print_usage(prog_name);
		}
		switch(argv[0][1]) {
		case 't':
			trace_path = argv[1];
			break;
		case 's':
			cache_size = atoi(argv[1]);
			if(cache_size < 1 || cache_size > 1024) {
				puts("Invalid cache size.");
				print_usage(prog_name);
			}
			break;
		case 'l':
			block_size = atoi(argv[1]);
			if(block_size < 4 || block_size > 256) {
				puts("Invalid block size.");
				print_usage(prog_name);
			}
			break;
		case 'a':
			associativity = atoi(argv[1]);
			if(associativity != 0 || associativity != 1) {
				puts("Invalid associativity value.");
				print_usage(prog_name);
			}
			break;
		case 'r':
			replacement = atoi(argv[1]);
			if(replacement != FIFO || replacement != LRU) {
				puts("Invalid replacement value.");
				print_usage(prog_name);
			}
			break;
		default:
			printf("Unknown option %s.\n", argv[0]);
			print_usage(prog_name);
		}

		argc -= 2;
		argv += 2;
	}
	if(trace_path == NULL || cache_size == -1 || block_size == -1 || associativity == -1 || replacement == -1) {
		puts("Miss arguments.");
		print_usage(prog_name);
	}
}

int main(int argc, char *argv[]) {
	arg_fetch(argc, argv);

	return 0;
}
