#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "map.h"
#include "tile.h"
#include <SDL_ttf.h>

/* class Pathfinder
This class deals with pathfinding.
*/

class Graphics;

class Pathfinder {
public:
	Pathfinder();
	Pathfinder(Map* mapP, Graphics* graphics);

	~Pathfinder();

	void testDrawTiles();

	void findPath(Tile* start, Tile* end);

private:
	Map* _mapP;
	Graphics* _graphicsP;
	TTF_Font* _font;
};

#endif