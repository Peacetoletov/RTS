#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <SDL_ttf.h>

#include <mutex>
#include <condition_variable>
#include <queue>
#include <stack>

/* class Pathfinder
This class deals with pathfinding.
*/

class Graphics;
class Tile;
class PathParameters;
class Map;

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
	std::stack<Tile*> A_Star(Tile* start, Tile* target, bool canFly);

	/* void threadStart
	This is where a new thread starts.
	*/
	void threadStart();

	//_pathParametersQueue functions using mutex
	void pushPathParameters(PathParameters* parameters);
	void popPathParameters();
	PathParameters* getFrontPathParameters();

	//Getters
	Map* getMapP();
	std::condition_variable* getCondP();
	/*
	Tile* getStartTileP();
	Tile* getTargetTileP();
	*/

private:
	Map* _mapP;
	Graphics* _graphicsP;
	TTF_Font* _font;

	std::mutex _muWaiter;
	std::mutex _mu;
	std::condition_variable _cond;
	
	//This queue is shared between 2 threads and needs to be accessed only using mutex
	std::queue<PathParameters*> _pathParametersQueue;

};

#endif