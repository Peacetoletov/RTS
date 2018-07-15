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

	/* TODO: Create a goal-based Dijkstra pathfinding algorithm
	In case the game starts lagging when using this algorithm on very big maps
	(let's say twice the size of an edge of the biggest map in the game),
	I will use breadth first search instead of Dijkstra. It's less precise,
	meaning sometimes if won't choose the shortest path, but it should be much
	more efficient because it doesn't need any sorting.

	Nevermind, the goal-based vector field is useless for me because I'm using a
	dynamic map. Units could get stuck way too easily. Instead, I will use a*
	for small groups ( < 10 units) individually. For bigger groups, I may
	need to use some sort of flocking.
	*/

private:
	Map* _mapP;
	Graphics* _graphicsP;
	TTF_Font* _font;
};

#endif