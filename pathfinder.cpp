#include "pathfinder.h"

#include "graphics.h"
#include "globals.h"
#include "comparator.h"
#include "pathparameters.h"
#include "tile.h"
#include "map.h"
#include "unit.h"
#include <algorithm>		//std::sort
#include <queue>			//std::priority_queue
#include <stack>			//std::stack

#include <vector>

#include <iostream>

/* class Pathfinder
This class deals with pathfinding.
*/

Pathfinder::Pathfinder() {}

Pathfinder::~Pathfinder() {
	
}

void Pathfinder::initMap(Map* mapP) {
	_mapP = mapP;
}

std::stack<Tile*> Pathfinder::bidirectionalDijkstra(Tile* start, Tile* target, Unit::Type type) {
		
	/* Create a vector of analyzed tiles.
	Will be used after the path is found to loop through all the analyzed tiles to reset them.
	This is common to both directions.
	*/
	std::vector<Tile*> analyzedTiles;

	/* Difference between visited and analyzed tiles
	Analyzed tiles haven't been visited yet. They were analyzed as a neighbour to a visited tile.
	Visited tiles have been previously analyzed and were the best candidates to be visited -
	they had the lowest F value out of all the analyzed tiles.
	*/

	/* Create 2 priority queues of open tiles, one for each direction
	Open tiles are candidates to be visited.
	Visited tiles are removed from the vector.
	*/
	std::priority_queue<Tile*, std::vector<Tile*>, Comparator> openTiles0;			//From start		
	std::priority_queue<Tile*, std::vector<Tile*>, Comparator> openTiles1;			//From target
	std::priority_queue<Tile*, std::vector<Tile*>, Comparator> openTiles[2] = { openTiles0 , openTiles1 };

	/* struct PossiblePath has 3 variables: total length of the path(total G), end tile of one direction,
	end tile of the other direction(the 2 end tiles are next to each other, because that's where the directions intersect).
	Whenever I find a new path, it's compared with this path's G to determine which one is better.
	*/
	PossiblePath currentBestPath;
	currentBestPath.totalG = INT_MAX;

	//The final path will be stored in this stack
	std::stack<Tile*> finalPath;

	//Initialize the algorithm - add start and target to analyzed and open tiles, set their G to 0
	bdInit(start, target, analyzedTiles, openTiles);

	//Begin the loop
	bool pathFound = false;
	Tile* currentTile = start;
	bool dirStart = true;				//True when the direction is from the start, false when from the target

	/* Edge case - if the start tile and target tile are right next to each other, the algortihm would fail to find the
	shortest path. To get around this, I will check all neighbours of the start tile and if one of the neighbours
	is the target, I will skip the normal algorithm and just return here.
	*/
	if (bdAreStartAndTargetNeighbours(start, target, finalPath)) {
		resetAnalyzedTiles(analyzedTiles);
		return finalPath;
	}

	//Attempt to find a path (not necessarily the best path)
	while (!pathFound) {
		currentTile = bdInitNewIteration(dirStart, currentTile, openTiles);
		if (currentTile == nullptr) {
			//Out of open tiles, path not found. This will return an empty stack
			break;
		}
		//Analyze neighbours	
		bdAnalyzeNeighbours(currentTile, dirStart, currentBestPath, pathFound, type, analyzedTiles, openTiles);
	}

	if (pathFound) {
		//Check all open tiles of the direction that found a path. If I find a better path, replace the current one.
		bdCheckForBetterPath(dirStart, currentBestPath, openTiles);

		//Join the directions together (reverse the pointers to parents of one direction).
		bdJoinDirectionsTogether(currentBestPath, target, start, finalPath);
	}

	//Reset all analyzed tiles
	resetAnalyzedTiles(analyzedTiles);

	return finalPath;
}

void Pathfinder::dijkstraForGroups(std::vector<Unit*>* unitsP, Tile* target) {
	/* This algorithm starts at the target and goes the usual Dijkstra way. It stops when it assigns a parent tile
	to all tiles that units in the group stand on. This way, I create a vector field that is specific to this group
	because of unique id.

	After all the important tiles are assigned, it looks at the unit that got its tile assigned first. This unit is closest
	to the target and will become a leader. I create a path (as a stack) from leader to the target and give this path
	to all units in the group.

	All units will try to go the same path as the leader as long as possible. Once it becomes impossible, they start ignoring
	leader's path and instead start following the vector field.

	If the vector field doesn't have the tile that the unit is currently standing on (can sometimes happen), it will calculate
	a path using bidirectional Dijkstra, as if it weren't part of any group.

	If a unit didn't have to change its path from leader's steps to vector field and got to the target only using leader's path,
	I will check the vector field once it reaches its destination. If it points to a tile that isn't occupied, I will move the
	unit in that direction until it gets stopped by an occupied tile. This is important when I select 2 units far from each other.

	This algorithm only works for land units but it wouldn't be difficult to also include air units.	
	*/

	//Before I do anything here, I need to refactor bidirectionalDijkstra
}

void Pathfinder::threadStart() {

	while (true) {
		std::unique_lock<std::mutex> locker(_muWaiter);
		if (_pathParametersQueue.size() == 0) {
			_cond.wait(locker);
		}
		
		std::cout << "Starting search. " << std::endl;
		auto start = std::chrono::system_clock::now();
		
		/* Stop the unit before calculating path to avoid problems.
		If I don't stop the unit, it might move to another tile. If that tile happens to be the first tile to visit
		on the newly calculated path, the unit wouldn't move because the tile is occupied (by itself).
		*/
		PathParameters* parameters = getFrontPathParameters();
		Unit* unit = (*(parameters->getUnitsP()))[0];		//This needs to be changed when I implement group pathfinding
		unit->setWantsToMove(false);		

		std::cout << "Id of this group is " << parameters->getGroupId() << std::endl;

		Tile* startTile = _mapP->getTilesP()[unit->getCurrentTileP()->getId()];
		std::stack<Tile*> path = bidirectionalDijkstra(startTile, parameters->getTargetP(), unit->getType());

		if (path.size() != 0) {
			unit->setPath(path);
			unit->setWantsToMove(true);		
		}
		
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<float> diff = end - start;
		std::cout << "Search finished. " << floor(diff.count() * 1000) << " milliseconds elapsed." << std::endl;
		

		delete parameters;
		popPathParameters();
		locker.unlock();
	}
	
	
}

int Pathfinder::getIncrementedCurrentGroupId() {
	//currentGroupId ranges from 0 to 99, then loops back to 0.
	if (_currentGroupId < 99) {
		_currentGroupId++;
	}
	else {
		_currentGroupId = 0;
	}
	return _currentGroupId;
}

void Pathfinder::pushPathParameters(PathParameters* parameters) {
	std::unique_lock<std::mutex> locker(_mu);
	_pathParametersQueue.push(parameters);
	locker.unlock();
}

void Pathfinder::popPathParameters() {
	std::unique_lock<std::mutex> locker(_mu);
	_pathParametersQueue.pop();
	locker.unlock();
}

PathParameters* Pathfinder::getFrontPathParameters() {
	std::unique_lock<std::mutex> locker(_mu);
	return _pathParametersQueue.front();
}

Map* Pathfinder::getMapP() {
	return _mapP;
}

std::condition_variable* Pathfinder::getCondP() {
	return &_cond;
}

//PRIVATE
//bidirectionalDijkstra (bd)
void Pathfinder::bdUpdatePathIfBetter(Tile* currentTile, Tile* neighbour, PossiblePath& currentBestPath, bool dirStart) {
	int G_increase = currentTile->isNeighbourDiagonal(neighbour) ? 14 : 10;
	int totalG = currentTile->getG() + neighbour->getG() + G_increase;
	if (totalG < currentBestPath.totalG) {
		currentBestPath.totalG = totalG;
		if (dirStart) {
			currentBestPath.path1End = currentTile;
			currentBestPath.path2End = neighbour;
		}
		else {
			currentBestPath.path1End = neighbour;
			currentBestPath.path2End = currentTile;
		}
		
	}
}

void Pathfinder::bdInit(Tile* start, Tile* target, std::vector<Tile*>& analyzedTiles, std::priority_queue<Tile*, std::vector<Tile*>, Comparator> openTiles[]) {
	//Set up the start tiles
	start->setG(0);
	target->setG(0);

	//Add the start tiles to openTiles and analyzedTiles
	analyzedTiles.push_back(start);
	analyzedTiles.push_back(target);
	openTiles[0].push(start);
	openTiles[1].push(target);
}

bool Pathfinder::bdAreStartAndTargetNeighbours(Tile* start, Tile* target, std::stack<Tile*>& finalPath) {
	//Returns true if they are neighbours, false if they aren't
	std::vector<Tile*>* neighbours = start->getNeighboursP();
	for (int i = 0; i < neighbours->size(); i++) {
		if ((*neighbours)[i]->getId() == target->getId()) {
			//Put the tile into the finalPath stack
			finalPath.push(target);
			return true;
		}
	}
	return false;
}

Tile* Pathfinder::bdInitNewIteration(bool& dirStart, Tile* currentTile, std::priority_queue<Tile*, std::vector<Tile*>, Comparator> openTiles[]) {
	//Switch the direction
	dirStart = !dirStart;

	/* Choose the most suitable tile to visit and remove the pointer to the current 
	tile from the openTiles vector, as I'm about to visit the tile.
	*/
	int index = dirStart ? 0 : 1;
	if (openTiles[index].size() != 0) {
		currentTile = openTiles[index].top();
		openTiles[index].pop();
	}
	else {
		//Out of open tiles, path not found. BD will return an empty stack.
		return nullptr;
	}

	//Set the current tile as visited
	currentTile->setWasVisited(true);

	return currentTile;
}

void Pathfinder::bdAnalyzeNeighbours(Tile* currentTile, bool& dirStart, PossiblePath& currentBestPath, bool& pathFound, 
		Unit::Type type, std::vector<Tile*>& analyzedTiles, std::priority_queue<Tile*, std::vector<Tile*>, Comparator> openTiles[]) {
	
	//Analyzes neighbours of the current tile
	std::vector<Tile*>* neighbours = currentTile->getNeighboursP();
	for (int i = 0; i < neighbours->size(); i++) {
		/* If the neighbour tile was already checked in this direction, skip it.
		If the unit cannot move through the neighbour tile, also skip it.
		If the neighbour tile was checked in the other direction, the path has been found.
		*/

		/* Check if the neighbour was analyzed. If it was, it might be a bridge to tiles from the other direction, 
		meaning that a path has been found.
		*/
		if ((*neighbours)[i]->getG() != INT_MAX) {
			/* Here, I need to keep checking all the neighbour tiles around this one to check if they aren't better
			in terms of path length. I can't break here after finding 1 path. After I check all the neighbours,
			I can finally leave this while loop and check all the other open tiles for possibly better paths.
			*/
			bdCheckPathFound(dirStart, (*neighbours)[i], currentTile, currentBestPath, pathFound);
		}
		/* This neighbour tile hasn't been analyzed. If it is accessible, assign value to its variables
		(G, parent, direction) and push it to the corresponding openList queue and analyzedTiles vector.
		*/
		else if ((*neighbours)[i]->isAvailableForPathfinding(type)) {
			bdAssignValuesToTile(currentTile, (*neighbours)[i], dirStart);
			bdPushTile(dirStart, (*neighbours)[i], analyzedTiles, openTiles);
		}
	}
}

void Pathfinder::bdCheckPathFound(bool dirStart, Tile* neighbour, Tile* currentTile, PossiblePath& currentBestPath, bool& pathFound) {
	if (dirStart && neighbour->getDirection() == Tile::Direction::TARGET) {

		//If this path is better than the current one (the first path found always is), update it to the new path.
		bdUpdatePathIfBetter(currentTile, neighbour, currentBestPath, dirStart);
		pathFound = true;
	}
	else if (!dirStart && neighbour->getDirection() == Tile::Direction::START) {

		//If this path is better than the current one (the first path found always is), update it to the new path.
		bdUpdatePathIfBetter(currentTile, neighbour, currentBestPath, dirStart);
		pathFound = true;
	}
}

void Pathfinder::bdAssignValuesToTile(Tile* currentTile, Tile* neighbour, bool dirStart) {
	//Set G value
	/* I first need to check if the neighbour tile is diagonal or not.
	If it's diagonal, I would add 14 to the current G, otherwise 10.
	I only change the G value if the new value would be smaller than
	the current one.

	POSSIBLE OPTIMIZATION:
	Instead of checking this all the time, I can assign another vector to each tile
	that contains information about each neighbour (whether or not it's diagonal).
	I would access this infomation based on the index in the vector.
	Is neighbour[i] diagonal? I look at isDiagonal[i] and boom! I don't need to calculate
	it over and over again.
	I tested it and found out that this would speed it up by 10%.
	*/
	int G_increase = currentTile->isNeighbourDiagonal(neighbour) ? 14 : 10;
	neighbour->setG(currentTile->getG() + G_increase);

	//Set the parent
	neighbour->setParentP(currentTile);

	//Set the direction
	if (dirStart) {
		neighbour->setDirection(Tile::Direction::START);
	}
	else {
		neighbour->setDirection(Tile::Direction::TARGET);
	}
}

void Pathfinder::bdPushTile(bool dirStart, Tile* neighbour, std::vector<Tile*>& analyzedTiles, std::priority_queue<Tile*, std::vector<Tile*>, Comparator> openTiles[]) {
	//Add this tile (neighbour) to the vector of analyzed and open tiles
	analyzedTiles.push_back(neighbour);
	if (dirStart) {
		openTiles[0].push(neighbour);
	}
	else {
		openTiles[1].push(neighbour);
	}
}

void Pathfinder::bdCheckForBetterPath(bool dirStart, PossiblePath& currentBestPath, std::priority_queue<Tile*, std::vector<Tile*>, Comparator> openTiles[]) {
	int index = dirStart ? 0 : 1;
	Tile* currentTile;
	while (openTiles[index].size() != 0) {
		currentTile = openTiles[index].top();
		openTiles[index].pop();

		/* Check all neighbours. Select the ones that were visited. If they have the opposite direction,
		they may be a part of the best path. In that case, I need to check the total G of that path and
		compare it with the current best path's total G. If it's lower, this new path is better.
		*/
		std::vector<Tile*>* neighbours = currentTile->getNeighboursP();
		for (int i = 0; i < neighbours->size(); i++) {
			if ((*neighbours)[i]->getG() != INT_MAX) {
				if (dirStart && (*neighbours)[i]->getDirection() == Tile::Direction::TARGET) {
					bdUpdatePathIfBetter(currentTile, (*neighbours)[i], currentBestPath, dirStart);
				}
				else if (!dirStart && (*neighbours)[i]->getDirection() == Tile::Direction::START) {
					bdUpdatePathIfBetter(currentTile, (*neighbours)[i], currentBestPath, dirStart);
				}
			}
		}
	}
}

void Pathfinder::bdJoinDirectionsTogether(PossiblePath& currentBestPath, Tile* target, Tile* start, std::stack<Tile*>& finalPath) {
	/* Join the directions together (reverse the pointers to parents of one direction).
	Reverse the target direction and add the target tile, then put everything into the stack.
	*/
	Tile* currentTile = currentBestPath.path2End;			//current tile to reverse
	Tile* previousTile = currentBestPath.path1End;
	Tile* nextTile = nullptr;
	//Loop through the all tiles in the path with the "target" direction. This does not include the target tile.
	while (currentTile->getId() != target->getId()) {
		//Add the target (only if the current tile is next to the target)
		if (currentTile->getParentP()->getId() == target->getId()) {
			target->setParentP(currentTile);
		}

		//Define next tile
		nextTile = currentTile->getParentP();

		//Reverse the parent pointer
		currentTile->setParentP(previousTile);

		//Define previous tile and current tile
		previousTile = currentTile;
		currentTile = nextTile;
	}

	//Put all tiles into a stack, starting from the target
	currentTile = target;
	while (currentTile->getId() != start->getId()) {
		finalPath.push(currentTile);
		currentTile = currentTile->getParentP();
	}
}

//Can be used in multiple functions
void Pathfinder::resetAnalyzedTiles(std::vector<Tile*>& analyzedTiles) {
	//Reset all analyzed tiles
	for (int i = 0; i < analyzedTiles.size(); i++) {
		analyzedTiles[i]->reset();
	}
}

//Deprecated
void Pathfinder::sortedTileInsert(std::vector<Tile*>& openTiles, Tile* tile) {
	
	int currentPos = openTiles.size() / 2;		//this is what position I'm currently looking at
	int portion = openTiles.size() / 2;			//this can be 1/2, 1/4, 1/8, 1/16... of the size
	while (true) {

		//Edge case #1: myVector.size() = 1
		//In this case, currentPos is always 0 and the tile will be inserted at the front, no matter its F value
		if (openTiles.size() == 1) {
			if (tile->getF() > openTiles[0]->getF()) {
				openTiles.insert(openTiles.begin(), tile);
			}
			else {
				openTiles.push_back(tile);
			}
			break;
		}

		//Edge case #2: new F is bigger than anything else
		if (currentPos == 0) {
			//std::cout << "new F is bigger than anything else!" << std::endl;
			openTiles.insert(openTiles.begin(), tile);
			break;
		}

		//Edge case #2: new F is smaller than anything else
		if (currentPos == openTiles.size()) {
			//std::cout << "new F is smaller than anything else!" << std::endl;
			openTiles.push_back(tile);
			break;
		}

		//Is this the right place?
		//Is the left F bigger or equal to new F and the right F smaller or equal to new F?
		if (openTiles[currentPos - 1]->getF() >= tile->getF() && openTiles[currentPos]->getF() <= tile->getF()) {
			openTiles.insert(openTiles.begin() + currentPos, tile);
			break;
		}

		//Split this half into 2 halves. If it would become 0, make it 1 instead.
		portion = (portion / 2) == 0 ? 1 : portion / 2;

		//Check which way to go
		if (tile->getF() < openTiles[currentPos]->getF()) {
			//Go right
			//Add the portion to the current position
			currentPos += portion;
		}
		else {
			//Go left
			//Subtract the portion from the current position
			currentPos -= portion;
		}

	}
	
}

void Pathfinder::updateTileInVector(std::vector<Tile*>& openTiles, int tileId) {
	//Find the tile I want to update in the vector
	Tile* tileP = nullptr;
	int pos;
	for (int i = 0; i < openTiles.size(); i++) {
		if (openTiles[i]->getId() == tileId) {
			tileP = openTiles[i];
			pos = i;
			break;
		}
	}
	if (tileP == nullptr) {
		std::cout << "Error! Couldn't find the tile with id " << tileId << " in the vector." << std::endl;
		return;
	}

	//Erase it from the vector
	openTiles.erase(openTiles.begin() + pos);

	//Insert it on the right position
	sortedTileInsert(openTiles, tileP);
}

std::stack<Tile*> Pathfinder::A_Star(Tile* start, Tile* target, bool canFly) {

	/* IMPORTANT
	As I'm currently not planning on using the A* pathfinding algorithm at all, this function is outdated
	and it's possible it doesn't work anymore, or it might behave unexpectedly!
	*/

	std::cout << "WARNING: Use of A*" << std::endl;

	/* Create a vector of analyzed tiles
	Will be used after the path is found to loop through all the analyzed tiles to reset them.
	*/
	std::vector<Tile*> analyzedTiles;

	/* Difference between visited and analyzed tiles
	Analyzed tiles haven't been visited yet. They were analyzed as a neighbour to a visited tile.
	Visited tiles have been previously analyzed and were the best candidates to be visited -
	they had the lowest F value out of all the analyzed tiles.
	*/

	//Create a vector of open tiles
	/*
	Open tiles are the tiles which are candidates to be visited.
	Visited tiles are removed from the vector.
	*/
	std::vector<Tile*> openTiles;

	//The final path will be stored in this stack
	std::stack<Tile*> finalPath;

	//Set up the start tile
	start->setG(0);
	start->setH(start->calculateH(target));

	//Add the start tile to openTiles and analyzedTiles
	analyzedTiles.push_back(start);
	openTiles.push_back(start);

	//Begin the loop
	bool pathFound = false;
	Tile* currentTile = nullptr;

	while (!pathFound) {

		//Choose the most suitable tile to visit
		if (openTiles.size() != 0) {
			//Lowest F value tiles (most suitable) will be at the end of the vector;
			currentTile = openTiles.back();
		}
		else {
			//Out of open tiles, path not found. This will return an empty stack
			break;
		}

		//Remove the pointer to the current tile from the openTiles vector, as I'm about to visit the tile
		openTiles.pop_back();

		//Set the current tile as visited
		currentTile->setWasVisited(true);

		//Analyze neighbours		
		std::vector<Tile*>* neighbours = currentTile->getNeighboursP();
		for (int i = 0; i < neighbours->size(); i++) {

			//If the neighbour tile was already checked, skip it.
			//If the unit cannot move through the neighbour tile, also skip it.

			/* TODO: Right now, I skip air unit accessible tiles without checking if the unit
			is of air type. Add this check.
			*/

			if (!(*neighbours)[i]->getWasVisited() && (*neighbours)[i]->getTerrainType() == Tile::TerrainAvailability::ALL) {

				//This can happen multiple times per tile

				//Set G value
				/*
				I first need to check if the neighbour tile is diagonal or not.
				If it's diagonal, I would add 14 to the current G, otherwise 10.
				I only change the G value if the new value would be smaller than
				the current one.
				*/

				/* POSSIBLE OPTIMIZATION:
				Instead of checking this all the time, I can assign another vector to each tile
				that contains information about each neighbour (whether or not it's diagonal).
				I would access this infomation based on the index in the vector.
				Is neighbour[i] diagonal? I look at isDiagonal[i] and boom! I don't need to calculate
				it over and over again.

				I tested it and found out that this would speed it up by 10 %.
				*/

				int G_increase = currentTile->isNeighbourDiagonal((*neighbours)[i]) ? 14 : 10;

				if (currentTile->getG() + G_increase < (*neighbours)[i]->getG()) {

					//If I update the G (therefore F as well), I need to update the position of the tile in openList
					bool update = false;
					if ((*neighbours)[i]->getG() != INT_MAX) {
						//This gets called if this tile already is in the openTiles vector. This needs to be before setting the G.
						update = true;
					}

					(*neighbours)[i]->setG(currentTile->getG() + G_increase);

					if (update) {
						updateTileInVector(openTiles, (*neighbours)[i]->getId());
					}

					//Set the parent (only if the new G is smaller than the previous G)
					(*neighbours)[i]->setParentP(currentTile);

				}

				//This can only happen once per tile
				if ((*neighbours)[i]->getH() == INT_MAX) {

					//Set H value
					int H = (*neighbours)[i]->calculateH(_mapP->getTilesP()[target->getId()]);
					(*neighbours)[i]->setH(H);

					//If H == 0, we found the path and can break the loop.
					if (H == 0) {
						pathFound = true;
					}

					//Add this tile to the vector of analyzed and open tiles
					analyzedTiles.push_back((*neighbours)[i]);
					sortedTileInsert(openTiles, (*neighbours)[i]);
				}
			}
		}

	}

	/* Put all the tiles that are included in the path to a stack and pass the stack to the unit.
	If the path wasn't found, it will return an empty stack.
	*/
	if (pathFound) {
		currentTile = target;
		while (currentTile->getId() != start->getId()) {
			finalPath.push(currentTile);
			currentTile = currentTile->getParentP();
		}
	}

	//Reset all analyzed tiles
	for (int i = 0; i < analyzedTiles.size(); i++) {
		analyzedTiles[i]->reset();
	}

	return finalPath;
}
