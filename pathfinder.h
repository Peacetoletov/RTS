#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <SDL_ttf.h>

#include <mutex>
#include <condition_variable>
#include <queue>
#include <stack>

#include "unit.h"			//necessary because of Unit::Type enum

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

	//TODO: Implement bidirectional Dijkstra pathfinding algortihm
	std::stack<Tile*> bidirectionalDijkstra(Tile* start, Tile* target, Unit::Type type);

	//struct PossiblePath is used in bidirectionalDijkstra
	struct PossiblePath {
		int totalG;
		Tile* path1End;
		Tile* path2End;
	};

	/* A* pathfinding algorithm
	TODO: all the TODOs defined in the function implementation
	*/
	std::stack<Tile*> A_Star(Tile* start, Tile* target, bool canFly);

	//This is where the pathfinder thread starts.
	void threadStart();

	//_pathParametersQueue functions using mutex
	void pushPathParameters(PathParameters* parameters);
	void popPathParameters();
	PathParameters* getFrontPathParameters();

	//Getters
	Map* getMapP();
	std::condition_variable* getCondP();

private:
	Map* _mapP;
	Graphics* _graphicsP;

	std::mutex _muWaiter;
	std::mutex _mu;
	std::condition_variable _cond;
	
	//This queue is shared between 2 threads and needs to be accessed only using mutex
	std::queue<PathParameters*> _pathParametersQueue;

	//METHODS
	/* Used in bidirectional Dijkstra. Checks if a new path is better than the current one. If yes, it replaces
	the current one with the new one.
	*/
	void updatePathIfBetter(Tile* currentTile, Tile* neighbour, PossiblePath& currentBestPath, bool dirStart);

	/* Takes a Tile* and inserts it into an already sorted vector of Tile*s (sorted by F) so that the vector remains 
	sorted after the insertion. Basically just puts in into the right place.
	*/
	void sortedTileInsert(std::vector<Tile*>& openTiles, Tile* tile);		

	// After updating tile's G, I call this function to update its position in the openTiles vector
	void updateTileInVector(std::vector<Tile*>& openTiles, int tileId);

};

#endif