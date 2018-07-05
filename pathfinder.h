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
	Pathfinder(Map* mapP, Graphics* graphics);

	void testDrawTiles(float tileSize);

	void findPath(Tile* start, Tile* end);

private:
	Map* _mapP;
	Graphics* _graphicsP;
};

#endif