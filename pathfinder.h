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
class Comparator;

/* TODO
Create a goal-based pathfinding algorithm that can be used for navigating large groups of units (10+ units in a group).
A* should be good enough for groups smaller than 10 units.
*/

class Pathfinder {
public:
	Pathfinder();

	~Pathfinder();

	void initMap(Map* mapP);

	//Bidirectional Dijkstra pathfinding algorithm used for individual units
	//std::stack<Tile*> bidirectionalDijkstra(Tile* start, Tile* target, Unit::Type type);
	std::stack<Tile*> bidirectionalDijkstra(Unit* unit, Tile* target);

	//Dijkstra pathfinding algorithm used for individual units
	std::stack<Tile*> dijkstra(Unit* unit, Tile* target);

	//Dijkstra pathfinding algorithm used for groups of units
	void dijkstraForGroups(std::vector<Unit*> units, Tile* target, int groupId);

	//struct PossiblePath is used in bidirectionalDijkstra
	struct PossiblePath {
		int totalG;
		Tile* path1End;
		Tile* path2End;
	};

	//A* pathfinding algorithm - deprecated
	std::stack<Tile*> A_Star(Tile* start, Tile* target, bool canFly);

	//This is where the pathfinder thread starts.
	void threadStart();

	//Increments currentGroupId and then returns it
	int getIncrementedCurrentGroupId();

	//_pathParametersQueue functions using mutex
	void pushPathParameters(PathParameters* parameters);
	void popPathParameters();
	PathParameters* getFrontPathParameters();

	//Getters
	Map* getMapP();
	std::condition_variable* getCondP();

private:
	Map* _mapP;

	std::mutex _muWaiter;
	std::mutex _mu;
	std::condition_variable _cond;
	
	//This queue is shared between 2 threads and needs to be accessed only using mutex
	std::queue<PathParameters*> _pathParametersQueue;

	//currentGroupId gives me the option to control multiple groups at the same time. It ranges from 0 to 99, then loops back to 0.
	int _currentGroupId = 0;

	//METHODS

	//Can be used in multiple functions
	//Resets analyzed tiles
	void resetAnalyzedTiles(std::vector<Tile*>& analyzedTiles);


	//bidirectionalDijkstra (bd)
	//Checks if a new path is better than the current one. If it is, it replaces the current one with the new one.
	void bdUpdatePathIfBetter(Tile* currentTile, Tile* neighbour, int neighbourIndex, PossiblePath& currentBestPath, bool dirStart);

	//Sets G of start and target tiles, adds them to the analyzedTiles vector and to openTiles priority queue
	void bdInit(Tile* start, Tile* target, std::vector<Tile*>& analyzedTiles, std::priority_queue<Tile*, std::vector<Tile*>, Comparator> openTiles[]);

	//If start and target tiles are neighbours, creates the path and returns true. Otherwise returns false.
	bool bdAreStartAndTargetNeighbours(Tile* start, Tile* target, std::stack<Tile*>& finalPath);

	//Initialize a new iteration of the while loop. Returns an updated currentTile, or nullptr if there are no available tiles.
	Tile* bdInitNewIteration(bool& dirStart, std::priority_queue<Tile*, std::vector<Tile*>, Comparator> openTiles[]);

	//Analyzes neighbours of the current tile.
	void bdAnalyzeNeighbours(Tile* currentTile, bool& dirStart, PossiblePath& currentBestPath, bool& pathFound,
		Unit::Type type, std::vector<Tile*>& analyzedTiles, std::priority_queue<Tile*, std::vector<Tile*>, Comparator> openTiles[]);

	//Checks if a path was found. If it was, it changes corresponding variables.
	void bdCheckPathFound(bool dirStart, Tile* neighbour, int neighbourIndex, Tile* currentTile, PossiblePath& currentBestPath, bool& pathFound);

	//Assigns values to currentTile's variables (G, parent, direction)
	void bdAssignValuesToTile(Tile* currentTile, Tile* neighbour, int neighbourIndex, bool dirStart);

	//Pushes currentTile to the analyzedTiles vector and a corresponding openTiles queue.
	void bdPushTile(bool dirStart, Tile* neighbour, std::vector<Tile*>& analyzedTiles, 
		std::priority_queue<Tile*, std::vector<Tile*>, Comparator> openTiles[]);

	//Checks all open tiles of the direction that found a path. If I find a better path, replace the current one.
	void bdCheckForBetterPath(bool dirStart, PossiblePath& currentBestPath, std::priority_queue<Tile*, std::vector<Tile*>, Comparator> openTiles[]);

	//Joins the directions together(reverses the pointers to parents of one direction)
	void bdJoinDirectionsTogether(PossiblePath& currentBestPath, Tile* target, Tile* start, std::stack<Tile*>& finalPath);


	//dijkstraForGroups (dfg)
	//Sets G of the target tile, adds it to the analyzedTiles vector and to openTiles priority queue.
	void dfgInit(Tile* target, std::vector<Tile*>& analyzedTiles, std::priority_queue<Tile*, std::vector<Tile*>, Comparator>& openTiles);

	//Analyzes all tiles which are occupied by units in the group and creates the vector field of pointers to parent tiles
	void dfgCreateVectorMap(bool& allTilesAnalyzed, Tile* currentTile, std::priority_queue<Tile*, std::vector<Tile*>, Comparator>& openTiles, 
		std::vector<Unit*> unitsCopy, std::vector<Tile*>& analyzedTiles, int groupId);

	//Initializes a new iteration. Returns next tile in the priority queue and sets it as visited.
	Tile* dfgInitNewIteration(std::priority_queue<Tile*, std::vector<Tile*>, Comparator>& openTiles);

	//Analyzes neighbours of the current tile.
	void dfgAnalyzeNeighbours(Tile* currentTile, std::vector<Tile*>& analyzedTiles,
		std::priority_queue<Tile*, std::vector<Tile*>, Comparator>& openTiles, int groupId);

	//Assigns values to currentTile's variables (G, parent).
	void dfgAssignValuesToTile(Tile* currentTile, Tile* neighbour, int neighbourIndex, int groupId);

	//Pushes currentTile to the analyzedTiles vector and the openTiles queue.
	void dfgPushTile(Tile* neighbour, std::vector<Tile*>& analyzedTiles,
		std::priority_queue<Tile*, std::vector<Tile*>, Comparator>& openTiles);

	//Check if all tiles that units in the group stand on are analyzed. 
	bool dfgAreAllTilesAnalyzed(std::vector<Unit*>& unitsCopy);

	//Assign groupId to each unit in the group
	void dfgAssignGroupId(std::vector<Unit*>& units, int groupId);

	//Choose the leader
	Unit* dfgChooseLeader(std::vector<Unit*> units);

	//Creates leader's path
	std::stack<int> dfgGetLeadersPathRelativeIdChange(Unit* leader, Tile* target, int groupId);

	//Set leader's path to each unit
	void dfgSetLeadersPath(std::vector<Unit*>& units, std::stack<int> leadersPathRelativeIdChange);

	//Deprecated
	/* Takes a Tile* and inserts it into an already sorted vector of Tile*s (sorted by F) so that the vector remains 
	sorted after the insertion. Basically just puts in into the right place.
	*/
	void sortedTileInsert(std::vector<Tile*>& openTiles, Tile* tile);		

	// After updating tile's G, I call this function to update its position in the openTiles vector
	void updateTileInVector(std::vector<Tile*>& openTiles, int tileId);

};

#endif