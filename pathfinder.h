#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "map.h"
#include <SDL_ttf.h>

#include <mutex>
#include <condition_variable>

/* class Pathfinder
This class deals with pathfinding.
*/

class Graphics;
class Tile;

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
	void A_Star(Tile* start, Tile* target);

	/* void threadStart
	This is where a new thread starts.
	*/
	void threadStart();

	//Sets _startTileP and _targetTileP
	void setTiles(Tile* startTileP, Tile* targetTileP);

	//Getters
	Map* getMapP();
	//std::mutex* getMuP();
	std::condition_variable* getCondP();
	Tile* getStartTileP();
	Tile* getTargetTileP();

private:
	Map* _mapP;
	Graphics* _graphicsP;
	TTF_Font* _font;

	std::mutex _muWaiter;
	std::mutex _mu;
	std::condition_variable _cond;

	//These Tile* pointers are shared between 2 threads and need to be accessed only using mutex
	Tile* _startTileP;
	Tile* _targetTileP;

};

#endif