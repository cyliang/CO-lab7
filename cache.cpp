#include "header.h"

Cache::Cache(int c, int b, int a, int r): cacheSize(c), blockSize(b), associativity(a), replacement(r) {
	setCount = cacheSize / blockSize / associativity;

	Set::setReplacement = r;
	Set::setAssociativity = a;
	sets = new Set[setCount];
}

Cache::~Cache() {
	delete[] sets;
}

bool Cache::getData(unsigned addr) {
	unsigned blockAddr = addr / blockSize;
	unsigned index = blockAddr % setCount;
	unsigned tag = blockAddr / setCount;

	/* To look for the data of that addr in the corresponding set. */
	return sets[index].getData(tag);
}
