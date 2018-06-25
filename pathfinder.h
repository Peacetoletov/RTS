#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "map.h"

/* class Pathfinder
This class deals with pathfinding.
*/

class Graphics;

class Pathfinder {
public:
	Pathfinder();
	Pathfinder(Map* mapp);

	void testDrawTiles(float tileSize, Graphics &graphics);
	void testPointer();
private:
	Map* _mapp;
};

#endif