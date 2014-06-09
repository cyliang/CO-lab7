#include <iostream>
#include <cstdlib>
#include <list>
using namespace std;

#define FIFO 0
#define LRU 1

char *trace_path = NULL;
int cache_size = -1, block_size = -1, associativity = -1, replacement = -1;

void arg_fetch(int argc, char *argv[]);

class Cache;
class Set;
class Block;

class Cache {
public:
	Cache();
	~Cache();
	void getData(unsigned addr);

private:
	Set *sets;
	int setCount;
};

class Set {
public:
	Set();
	void getData(unsigned tag);

private:
	list<Block> blocks;
};

class Block {
public:
	Block();
	bool getData(unsigned tag); 	// Check if this block contain the desire data.
	void replace(unsigned tag); 	// Replace the original data to the new one. 

private:
//	char *data; 			// Needn't to store the data in this assignment.
	bool valid;
	unsigned tag;
}


int main(int argc, char *argv[]) {
	arg_fetch(argc, argv);

	return 0;
}

void print_usage(const char *prog_name) {
	cout <<	"Usage: " << prog_name << " -t trace_file -s cache_size -l block_size -a associativity -r replacement\n"
	     << "arguments:\n"
	     << "\ttrace_file: The path to the input file which contains those memory address to access.\n"
	     << "\tcache_size: The size of the cache. 1~1024 (KByte).\n"
	     << "\tblock_size: The size of a cache block. 4~256 (Byte).\n"
	     << "associativity: The number of the way.\n"
	     << "replacement: 0 for FIFO and 1 for LRU." << endl;

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
				cout << "Invalid cache size.\n";
				print_usage(prog_name);
			}
			break;
		case 'l':
			block_size = atoi(argv[1]);
			if(block_size < 4 || block_size > 256) {
				cout << "Invalid block size.\n";
				print_usage(prog_name);
			}
			break;
		case 'a':
			associativity = atoi(argv[1]);
			if(associativity != 0 || associativity != 1) {
				cout << "Invalid associativity value.\n";
				print_usage(prog_name);
			}
			break;
		case 'r':
			replacement = atoi(argv[1]);
			if(replacement != FIFO || replacement != LRU) {
				cout << "Invalid replacement value.\n";
				print_usage(prog_name);
			}
			break;
		default:
			cout << "Unknown option" << argv[0] << ".\n";
			print_usage(prog_name);
		}

		argc -= 2;
		argv += 2;
	}
	if(trace_path == NULL || cache_size == -1 || block_size == -1 || associativity == -1 || replacement == -1) {
		cout << "Miss arguments.\n";
		print_usage(prog_name);
	}
}

Block::Block(): valid(false) {
}

bool Block::getData(unsigned tag) {
	if(valid && tag == this->tag) {
		cout << "hit\ntag: " << tag << "->" << tag << "\nvalid: 1->1\n";
		return true;
	}
	return false;
}

void Block::replace(unsigned tag) {
	cout << "miss\ntag: " << this->tag << "->" << tag << "\nvalid: " << valid << "->1\n";
	valid = true;
	this->tag = tag;
}

Set::Set() {
	blocks.resize(associativity);
}

void Set::getData(unsigned tag) {
	for(list<Block>::iterator it = blocks.begin(); it != blocks.end(); ++it) {
		if(it->getData(tag)) {
			if(replacement == LRU) {
				Block temp = *it;
				blocks.erase(it);
				blocks.push_back(temp);
			}

			return;
		}
	}

	Block temp = blocks.front();
	blocks.pop_front();
	temp.replace(tag);
	blocks.push_back(temp);
}

Cache::Cache() {
	setCount = cache_size / block_size / associativity;
	sets = new Set[setCount];
}

Cache::~Cache() {
	delete[] sets;
}

Cache::getData(unsigned addr) {
	unsigned block_addr = addr / block_size;
	unsigned index = block_addr % setCount;
	unsigned tag = block_addr / setCount;

	sets[index].getData(tag);
}
