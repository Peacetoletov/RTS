#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "map.h"
#include "tile.h"

/* class Pathfinder
This class deals with pathfinding.
*/

class Graphics;

class Pathfinder {
public:
	Pathfinder();
	Pathfinder(Map* mapP);

	void testDrawTiles(float tileSize, Graphics &graphics);

	void findPath(Tile* start, Tile* end);

private:
	Map* _mapP;
};

#endif