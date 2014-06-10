#include <iostream>
#include <fstream>
#include <cstdlib>
#include <list>
using namespace std;

#define FIFO 0
#define LRU 1

/* Arguments declare */
char *trace_path;
int cache_size = -1, block_size = -1, associativity = -1, replacement = -1;
ifstream trace_file;

void arg_fetch(int argc, char *argv[]); // To fetch arguments.

/* Forward declare */
class Cache;
class Set;
class Block;

/* Declare 'Cache', 'Set', and 'Block'.
 * Cache contains sets, and set contains blocks. */
class Cache {
public:
	Cache(); 			// Create the cache. (Create the sets of this cache)
	~Cache();
	bool getData(unsigned addr); 	// To check if the data of addr are cached. Cache if not.

private:
	Set *sets; 			// The array of all sets.
	int setCount; 			// The number of the sets in this cache.
};

class Set {
public:
	Set(); 				// Create the set. (Create the blocks in this set)
	bool getData(unsigned tag); 	// To check if the data of tag are in this set. Store if not.

private:
	list<Block> blocks; 		// Use list to store blocks to maintain the priority of replacement.
};

class Block {
public:
	Block(); 			// Create the block. (Initiallize valid bit to 0)
	bool getData(unsigned tag); 	// Check if this block contain the desire data.
	void replace(unsigned tag); 	// Replace the original data to the new one. 

private:
//	char *data; 			// Needn't to store the data in this assignment.
	bool valid; 			// The valid bit.
	unsigned tag; 			// The tag of this block.
};


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

	Cache cache;
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

/* Initiallize blocks with valid bit 0. */
Block::Block(): valid(false) {
}

bool Block::getData(unsigned tag) {
	if(valid && tag == this->tag) {
		/* Print hit information. */
		cout << "hit\ntag: " << tag << "->" << tag << "\nvalid: 1->1\n";
		return true;
	}
	return false;
}

/* This function is called only when miss. */
void Block::replace(unsigned tag) {
	/* Print miss information. */
	cout << "miss\ntag: " << this->tag << "->" << tag << "\nvalid: " << valid << "->1\n";
	valid = true;
	this->tag = tag;
}

Set::Set() {
	blocks.resize(associativity); 	// The number of blocks a set contains is 'associativity'
}

/* The blocks are maintained by list because only list can remove an element from the middle of it with low cost.
 * The block with neweast data is moved to the tail of the list.
 * The block of the front of the list is the next block to be replaced. */
bool Set::getData(unsigned tag) {
	for(list<Block>::iterator it = blocks.begin(); it != blocks.end(); ++it) {
		/* One of its blocks contains the data.
		 *  -> The data is hit in this set. */
		if(it->getData(tag)) {
			/* For LRU, if the block is hit, 
			 * that block would be moved to the tail of the list. */
			if(replacement == LRU) {
				Block temp = *it;
				blocks.erase(it);
				blocks.push_back(temp);
			}

			/* For FIFO, nothing needs to do if a block is hit. */

			return true;
		}
	}

	/* The data are not in this set,
	 * so replace one of its blocks to store the new data. */
	Block temp = blocks.front();
	blocks.pop_front();
	temp.replace(tag);
	blocks.push_back(temp);
	return false;
}

Cache::Cache() {
	setCount = cache_size / block_size / associativity;
	sets = new Set[setCount];
}

Cache::~Cache() {
	delete[] sets;
}

bool Cache::getData(unsigned addr) {
	unsigned block_addr = addr / block_size;
	unsigned index = block_addr % setCount;
	unsigned tag = block_addr / setCount;

	/* To look for the data of that addr in the corresponding set. */
	return sets[index].getData(tag);
}
