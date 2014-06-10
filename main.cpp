#include <iostream>
#include <cstdlib>
#include "header.h"
using namespace std;

/* Arguments declare */
char *trace_path;
int cache_size = -1, block_size = -1, associativity = -1, replacement = -1;
std::ifstream trace_file;

void arg_fetch(int argc, char *argv[]); // To fetch arguments.


int main(int argc, char *argv[]) {
	/* Fetch the arguments */
	arg_fetch(argc, argv);
	/* Print the specification of this cache. */
	cout << "Trace file: " << trace_path << '\n'
	     << "Cache size: " << cache_size << '\n'
	     << "Line size: " << block_size << '\n'
	     << "Associativity: " << associativity << '\n'
	     << "Replace: " << (replacement == LRU ? "LRU" : "FIFO") << '\n'
	     << "Number of blocks= " << cache_size / block_size << '\n'
	     << "Number of sets= " << cache_size / block_size / associativity << '\n';

	Cache cache(cache_size, block_size, associativity, replacement);
	int hitCount = 0, missCount = 0;

	trace_file.setf(ios::hex, ios::basefield);
	unsigned addr;
	while(trace_file >> addr) {
		if(cache.getData(addr)) // True for hit; false for miss.
			hitCount++;
		else
			missCount++;
	}

	/* Print the summery */
	cout << "Cache hits= " << hitCount << '\n'
	     << "Cache miss= " << missCount << '\n'
	     << "Miss rate: " << missCount * 100 / (missCount + hitCount) << '%' << endl;

	return 0;
}

/* This function is called when there is any error in arguments fetching.
 * It prints the usage of this program and terminates the execution. */
void print_usage(const char *prog_name) {
	cerr <<	"Usage: " << prog_name << " -t trace_file -s cache_size -l block_size -a associativity -r replacement\n"
	     << "arguments:\n"
	     << "\ttrace_file: The path to the input file which contains those memory address to access.\n"
	     << "\tcache_size: The size of the cache. 1~1024 (KByte).\n"
	     << "\tblock_size: The size of a cache block. 4~256 (Byte).\n"
	     << "\tassociativity: The number of the way.\n"
	     << "\treplacement: 0 for FIFO and 1 for LRU." << endl;

	exit(2);
}

/* Fetch all the arguments and check if any is invalid. */
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
			trace_file.open(trace_path);
			if(!trace_file) {
				cerr << "Cannot open file '" << trace_path << "'.\n";
				print_usage(prog_name);
			}
			break;
		case 's':
			cache_size = atoi(argv[1]);
			if(cache_size < 1 || cache_size > 1024) {
				cerr << "Invalid cache size.\n";
				print_usage(prog_name);
			}
			cache_size <<= 10; 	// Convert from 'KByte' to 'Byte'
			break;
		case 'l':
			block_size = atoi(argv[1]);
			if(block_size < 4 || block_size > 256) {
				cerr << "Invalid block size.\n";
				print_usage(prog_name);
			}
			break;
		case 'a':
			associativity = atoi(argv[1]);
			if(associativity > cache_size / block_size) {
				cerr << "Invalid associativity value.\n";
				print_usage(prog_name);
			}
			break;
		case 'r':
			replacement = atoi(argv[1]);
			if(replacement != FIFO && replacement != LRU) {
				cerr << "Invalid replacement value.\n";
				print_usage(prog_name);
			}
			break;
		default:
			cerr << "Unknown option" << argv[0] << ".\n";
			print_usage(prog_name);
		}

		argc -= 2;
		argv += 2;
	}
	if(!trace_file.is_open() || cache_size == -1 || block_size == -1 || associativity == -1 || replacement == -1) {
		cerr << "Miss arguments.\n";
		print_usage(prog_name);
	}
}

