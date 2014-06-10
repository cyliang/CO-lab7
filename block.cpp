#include "header.h"
#include <iostream>

/* Initiallize blocks with valid bit 0. */
Block::Block(): valid(false) {
}

bool Block::getData(unsigned tag) {
	if(valid && tag == this->tag) {
		/* Print hit information. */
		std::cout << "hit\ntag: " << tag << "->" << tag << "\nvalid: 1->1\n";
		return true;
	}
	return false;
}

/* This function is called only when miss. */
void Block::replace(unsigned tag) {
	/* Print miss information. */
	std::cout << "miss\ntag: " << this->tag << "->" << tag << "\nvalid: " << valid << "->1\n";
	valid = true;
	this->tag = tag;
}

