#ifndef COMPARATOR_H
#define COMPARATOR_H

class Tile;

class Comparator {
public:
	bool operator() (Tile* a, Tile* b);
};

#endif