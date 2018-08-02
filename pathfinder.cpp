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

//test
//#include <time.h>


/* class Pathfinder
This class deals with pathfinding.
*/

Pathfinder::Pathfinder() {}

Pathfinder::Pathfinder(Map* mapP, Graphics* graphicsP) :
	_mapP(mapP),
	_graphicsP(graphicsP)
{
	
}

Pathfinder::~Pathfinder() {
	
}

std::stack<Tile*> Pathfinder::bidirectionalDijkstra(Tile* start, Tile* target, bool canFly) {

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
	std::priority_queue<Tile*, std::vector<Tile*>, Comparator> openTiles1;			//From start		
	std::priority_queue<Tile*, std::vector<Tile*>, Comparator> openTiles2;			//From target

	/* struct PossiblePath has 3 variables: total length of the path(total G), end tile of one direction,
	end tile of the other direction(the 2 end tiles are next to each other, because that's where the directions intersect).
	Whenever I find a new path, it's compared with this path's G to determine which one is better.
	*/
	PossiblePath currentBestPath;
	currentBestPath.totalG = INT_MAX;

	//The final path will be stored in this stack
	std::stack<Tile*> finalPath;

	//Set up the start tiles
	start->setG(0);
	target->setG(0);

	//Add the start tiles to openTiles and analyzedTiles
	analyzedTiles.push_back(start);
	analyzedTiles.push_back(target);
	openTiles1.push(start);
	openTiles2.push(target);

	//Begin the loop
	bool pathFound = false;
	Tile* currentTile = nullptr;
	bool dirStart = true;				//True when the direction is from the start, false when from the target

	//Attempt to find a path (not necessarily the best path)
	while (!pathFound) {

		//Switch the direction
		dirStart = !dirStart;

		/* Choose the most suitable tile to visit and remove the pointer to the current 
		tile from the openTiles vector, as I'm about to visit the tile.
		*/
		if (dirStart) {
			if (openTiles1.size() != 0) {
				currentTile = openTiles1.top();
				openTiles1.pop();
			}
			else {
				//Out of open tiles, path not found. This will return an empty stack
				break;
			}
		}
		else {
			if (openTiles2.size() != 0) {
				currentTile = openTiles2.top();
				openTiles2.pop();
			}
			else {
				//Out of open tiles, path not found. This will return an empty stack
				break;
			}
		}

		//Set the current tile as visited
		currentTile->setWasVisited(true);

		
		std::cout << "Current tile c|r " << _mapP->idToColumn(currentTile->getId()) <<
			"|" << _mapP->idToRow(currentTile->getId()) << std::endl;
			
		
		//Analyze neighbours		
		std::vector<Tile*>* neighbours = currentTile->getNeighboursP();
		for (int i = 0; i < neighbours->size(); i++) {

			/* If the neighbour tile was already checked in this direction, skip it.
			If the neighbour tile was checked in the other direction, the path has been found.
			If the unit cannot move through the neighbour tile, also skip it.
			*/

			/* TODO: Right now, I skip air unit accessible tiles without checking if the unit
			is of air type. Add this check.
			*/

			//Check if the directions intersect, therefore path has been found
			if ((*neighbours)[i]->getWasVisited()) {
				/* Here, I need to keep checking all the neighbour tiles around this one to check if they aren't better
				in terms of path length. I can't break here after finding 1 path. After I check all the neighbours,
				I can finally leave this while loop and check all the other open tiles.
				*/

				//For some reason, it doesn't fullfil the condition below even in times when it should
				if (dirStart && (*neighbours)[i]->getDirection() == Tile::Direction::TARGET) {
					std::cout << "Found a path! 1" << std::endl;
					//If this path is better than the current one (the first path found always is), update it to the new path.
					int G_increase = currentTile->isNeighbourDiagonal((*neighbours)[i]) ? 14 : 10;
					int totalG = currentTile->getG() + (*neighbours)[i]->getG() + G_increase;
					if (totalG < currentBestPath.totalG) {
						currentBestPath.totalG = totalG;
						currentBestPath.path1End = currentTile;
						currentBestPath.path2End = (*neighbours)[i];
					}
					pathFound = true;
				}
				else if (!dirStart && (*neighbours)[i]->getDirection() == Tile::Direction::START) {
					std::cout << "Found a path! 2" << std::endl;
					//If this path is better than the current one (the first path found always is), update it to the new path.
					int G_increase = currentTile->isNeighbourDiagonal((*neighbours)[i]) ? 14 : 10;
					int totalG = currentTile->getG() + (*neighbours)[i]->getG() + G_increase;
					if (totalG < currentBestPath.totalG) {
						currentBestPath.totalG = totalG;
						currentBestPath.path1End = (*neighbours)[i];
						currentBestPath.path2End = currentTile;
					}
					pathFound = true;
				}
			}
			/* This neighbour tile hasn't been visited. If it wasn't analyzed and is accessible, set its variables
			(G, parent, direction) and push it to the corresponding openList queue.
			*/
			else if ((*neighbours)[i]->getTerrainType() == Tile::TerrainAvailability::ALL &&
				(*neighbours)[i]->getG() == INT_MAX) {

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
				(*neighbours)[i]->setG(currentTile->getG() + G_increase);

				//Set the parent
				(*neighbours)[i]->setParentP(currentTile);

				/* TODO
				Currently, it's not guaranteed to find the best path because it breaks the first time it finds a possible path.

				I could probably fix this by not stopping there, only putting it as the best path so far. Then, I would look
				at all the remaining tiles in openTiles of that particular direction
				*/

				/* TODO
				Currently, if I select the target right next to the start, it will probably fail to find the path.
				*/

				//Add this tile to the vector of analyzed and open tiles, set the direction
				analyzedTiles.push_back((*neighbours)[i]);
				if (dirStart) {
					openTiles1.push((*neighbours)[i]);
					(*neighbours)[i]->setDirection(Tile::Direction::START);

					/*
					std::cout << "Pushing a tile c|r " << _mapP->idToColumn((*neighbours)[i]->getId()) <<
						"|" << _mapP->idToRow((*neighbours)[i]->getId()) << " to openList1! It has G = " <<
						(*neighbours)[i]->getG() << std::endl;
						*/
				}
				else {
					openTiles2.push((*neighbours)[i]);
					(*neighbours)[i]->setDirection(Tile::Direction::TARGET);

					/*
					std::cout << "Pushing a tile c|r " << _mapP->idToColumn((*neighbours)[i]->getId()) << 
						"|" << _mapP->idToRow((*neighbours)[i]->getId()) << " to openList2! It has G = " << 
						(*neighbours)[i]->getG() << std::endl;
						*/
						
				}
			}
		}
		
	}

	//Test
	std::cout << "If there are no more messages, no path has been found. pathFound = " << pathFound << std::endl;
	if (pathFound) {
		std::cout << "A path has been found! It goes from middle to the start like this: " << std::endl;
		Tile* currentTileTest = currentBestPath.path1End;
		while (currentTileTest->getId() != start->getId()) {
			std::cout << "Column|Row: " << _mapP->idToColumn(currentTileTest->getId()) << "|" <<
				_mapP->idToRow(currentTileTest->getId()) << std::endl;
			currentTileTest = currentTileTest->getParentP();
		}
		std::cout << "And it goes from middle to target like this: " << std::endl;
		currentTileTest = currentBestPath.path2End;
		while (currentTileTest->getId() != target->getId()) {
			std::cout << "Column|Row: " << _mapP->idToColumn(currentTileTest->getId()) << "|" <<
				_mapP->idToRow(currentTileTest->getId()) << std::endl;
			currentTileTest = currentTileTest->getParentP();
		}

		std::cout << "This is the whole path." << std::endl;
	}

	//TODO: Check all the open tiles of the direction that found a path. If I find a better path, replace the current one.

	//TODO: Join the directions together (reverse the pointers to parents of one direction)

	return finalPath;
}

std::stack<Tile*> Pathfinder::A_Star(Tile* start, Tile* target, bool canFly) {

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

void Pathfinder::threadStart() {

	while (true) {
		std::unique_lock<std::mutex> locker(_muWaiter);
		if (_pathParametersQueue.size() == 0) {
			_cond.wait(locker);
		}
		
		/* TODO - fix a bug
		Sometimes, when I click too fast on one place, I get a message saying that the next tile is occupied. 
		I think this happens because the unit is in one place, I send a request for another path, and before the
		path is calculated, the unit moves. The first tile on the newly calculated path is the one that the unit
		is standing on right now. That's why it says it cannot move because the next tile is occupied.

		This is also the reason why when I send a new path request that's opposite of the path the unit is taking
		right now, it sometimes "jumps" a few tiles.

		Temporary (?) solution: When I select a new path, the unit will stop moving.
		*/
		
		std::cout << "Starting search. " << std::endl;
		auto start = std::chrono::system_clock::now();
		

		PathParameters* parameters = getFrontPathParameters();
		if (parameters->getAlgorithm() == PathParameters::Algorithm::A_Star) {
			Unit* unit = (*(parameters->getUnitsP()))[0];
			Tile* startTile = _mapP->getTilesP()[unit->getCurrentTileP()->getId()];
			//std::stack<Tile*> path = A_Star(startTile, parameters->getTargetP(), false);
			std::stack<Tile*> path = bidirectionalDijkstra(startTile, parameters->getTargetP(), false);

			if (path.size() != 0) {
				unit->setPath(path);
				unit->setWantsToMove(true);
			}
			
		}


		
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<float> diff = end - start;
		std::cout << "Search finished. " << floor(diff.count() * 1000) << " milliseconds elapsed." << std::endl;
		

		delete parameters;
		popPathParameters();
		locker.unlock();
	}
	
	
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
