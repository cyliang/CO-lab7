#include "header.h"

int Set::setReplacement, Set::setAssociativity;

Set::Set(): replacement(setReplacement) {
	blocks.resize(setAssociativity); 	// The number of blocks a set contains is 'associativity'
}

/* The blocks are maintained by list because only list can remove an element from the middle of it with low cost.
 * The block with neweast data is moved to the tail of the list.
 * The block of the front of the list is the next block to be replaced. */
bool Set::getData(unsigned tag) {
	for(std::list<Block>::iterator it = blocks.begin(); it != blocks.end(); ++it) {
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

