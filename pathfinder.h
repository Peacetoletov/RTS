#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "map.h"
#include "tile.h"
#include <SDL_ttf.h>

/* class Pathfinder
This class deals with pathfinding.
*/

class Graphics;

/* TODO
Create a goal-based pathfinding algorithm that can be used for navigating large groups of units (10+ units in a group).
A* should be good enough for groups smaller than 10 units.
*/

class Pathfinder {
public:
	Pathfinder();
	Pathfinder(Map* mapP, Graphics* graphics);

	~Pathfinder();

	void testDrawTiles();

	/* void A_Star
	A* pathfinding algorithm
	TODO: all the TODOs defined in the function implementation
	*/
	void A_Star(Tile* start, Tile* end);

private:
	Map* _mapP;
	Graphics* _graphicsP;
	TTF_Font* _font;

	//Methods
	static bool compareTilesF(Tile* a, Tile* b);
};

#endif