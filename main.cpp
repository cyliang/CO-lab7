#include <iostream>
#include <fstream>
#include <cstdlib>
#include <list>
using namespace std;

#define FIFO 0
#define LRU 1

char *trace_path;
int cache_size = -1, block_size = -1, associativity = -1, replacement = -1;
ifstream trace_file;

void arg_fetch(int argc, char *argv[]);

class Cache;
class Set;
class Block;

class Cache {
public:
	Cache();
	~Cache();
	bool getData(unsigned addr);

private:
	Set *sets;
	int setCount;
};

class Set {
public:
	Set();
	bool getData(unsigned tag);

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
};


int main(int argc, char *argv[]) {
	arg_fetch(argc, argv);
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
		if(cache.getData(addr))
			hitCount++;
		else
			missCount++;
	}

	cout << "Cache hits= " << hitCount << '\n'
	     << "Cache miss= " << missCount << endl;

	return 0;
}

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
			cache_size <<= 10;
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

bool Set::getData(unsigned tag) {
	for(list<Block>::iterator it = blocks.begin(); it != blocks.end(); ++it) {
		if(it->getData(tag)) {
			if(replacement == LRU) {
				Block temp = *it;
				blocks.erase(it);
				blocks.push_back(temp);
			}

			return true;
		}
	}

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

	return sets[index].getData(tag);
}
