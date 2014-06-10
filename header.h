#pragma once
#include <list>
#include <fstream>

#define FIFO 0
#define LRU 1

/* Forward declare */
class Cache;
class Set;
class Block;

/* Declare 'Cache', 'Set', and 'Block'.
 * Cache contains sets, and set contains blocks. */
class Cache {
public:
	/* Create the cache. (Create the sets of this cache) */
	Cache(int cacheSize, int blockSize, int associativity, int replacement);
	~Cache();
	bool getData(unsigned addr); 	// To check if the data of addr are cached. Cache if not.

private:
	Set *sets; 			// The array of all sets.
	int setCount; 			// The number of the sets in this cache.

	int cacheSize;
	int blockSize;
	int associativity;
	int replacement;
};

class Set {
public:
	Set(); 				// Create the set. (Create the blocks in this set)
	bool getData(unsigned tag); 	// To check if the data of tag are in this set. Store if not.

	static int setReplacement, setAssociativity;

private:
	std::list<Block> blocks;	// Use list to store blocks to maintain the priority of replacement.

	int replacement;
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
