#include "comparator.h"
#include "tile.h"

bool Comparator::operator() (Tile* a, Tile* b) {
	/*
	if (a->getF() < b->getF()) {
		return true;
	}
	else {
		return false;
	}
	*/

	//Not sure which order this is - descending or ascending 
	return (a->getG() > b->getG());
}