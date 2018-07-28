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

std::stack<Tile*> Pathfinder::A_Star(Tile* start, Tile* target, bool canFly) {

	/* HIGH PRIORITY TODO:
	If I select a long, almost straight path (only 1 tile above or below a straight line), it doesn't 
	find the best path.

	Found the cause of the problem. It's in the sorting of the priority queue. For some reason, the priority queue
	is not reliable.

	REAL LÖSUNG:
	I will use vector instead of priority queue to store the openTiles in. I will not sort it. Instead, whenever I insert
	an element into the vector (using the method insert), I will place it on the place where it belongs. I will use my
	awesome method of repeated splitting of the vector into halves until I end up between 2 values, left one being smaller or equal,
	and right one being bigger or equal. Once I find this place, I will insert the tile in there. 
	When I change a value, I will find it in the vector (O(n)), delete it using std::vector::erase and insert it again.

	Comparison with the old version of sorting the vector using std::sort after each time I add an element or change 
	an element's value:
	I have no idea how complex my awesome method of repeated splitting is but it must be more efficient than brute-force
	comparison with all possible tiles, so that means it must be less than O(n). The original sort was O(n log(n)).
	Finding the tile I'm looking to erase is O(n)
	By testing, I learned that the complexity of functions insert and erase is so small that I can safely ignore it.

	Whatever, this is extremely confusing. I guess I'll just try it and it will either work or it won't.
	*/

	//TODO: Remove the Comparator files

	//Create a vector of analyzed tiles
	//Will be used after the path is found to loop through all the analyzed tiles to reset them.
	std::vector<Tile*> analyzedTiles;

	/* Difference between visited and analyzed tiles
	Analyzed tiles haven't been visited yet. They were analyzed as a neighbour to a visited tile.
	Visited tiles have been previously analyzed and were the best candidates to be visited -
	they had the lowest F value out of all the analyzed tiles.
	*/

	//Create a vector of open tiles
	/*
	Open tiles are the tiles which are candidates to be visited. 
	Visited tiles are removed from the queue.
	*/
	std::vector<Tile*> openTiles;

	//The final path will be stored in this stack
	std::stack<Tile*> finalPath;

	//Set up the start tile
	start->setG(0);
	start->setH(start->calculateH(target));

	//Add the start tile openTiles and analyzedTiles
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
					if ((*neighbours)[i]->getG() != INT_MAX) {
						//TODO: This

					}

					(*neighbours)[i]->setG(currentTile->getG() + G_increase); 

					/*
					std::cout << "Tile " << _mapP->idToRow((*neighbours)[i]->getId()) << "|" <<
					_mapP->idToColumn((*neighbours)[i]->getId()) << " has F " <<
					(*neighbours)[i]->getF() << std::endl;
					*/

					//Set the parent
					//Only if the new G is smaller than the previous G
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
		
		
		std::cout << "Starting search. " << std::endl;
		auto start = std::chrono::system_clock::now();
		

		PathParameters* parameters = getFrontPathParameters();
		if (parameters->getAlgorithm() == PathParameters::Algorithm::A_Star) {
			Unit* unit = (*(parameters->getUnitsP()))[0];
			Tile* startTile = _mapP->getTilesP()[unit->getCurrentTileP()->getId()];
			std::stack<Tile*> path = A_Star(startTile, parameters->getTargetP(), false);

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

//TESTING
void Pathfinder::testSortedTileInsert(std::vector<int>& myVector, int myInt) {
	int currentPos = myVector.size() / 2;		//this is what position I'm currently looking at
	int portion = myVector.size() / 2;			//this can be 1/2, 1/4, 1/8, 1/16... of the size
	while (true) {

		//Edge case #1: myVector.size() = 1
		//In thise case, currentPos is always 0 and the int will be inserted at the front, no matter its value
		if (myVector.size() == 1) {
			if (myInt > myVector[0]) {
				myVector.insert(myVector.begin(), myInt);
			}
			else {
				myVector.push_back(myInt);
			}
			break;
		}

		//Edge case #2: myInt is bigger than anything else
		if (currentPos == 0) {
			//std::cout << "myInt is bigger than anything else!" << std::endl;
			myVector.insert(myVector.begin(), myInt);
			break;
		}

		//Edge case #3: myInt is smaller than anything else
		if (currentPos == myVector.size()) {
			//std::cout << "myInt is smaller than anything else!" << std::endl;
			myVector.push_back(myInt);
			break;
		}
		
		//Is this the right place?
		//Is the left one bigger or equal to my int and the right one smaller or equal to my int?
		if (myVector[currentPos - 1] >= myInt && myVector[currentPos] <= myInt) {		//might go out of bounds
			myVector.insert(myVector.begin() + currentPos, myInt);
			break;
		}

		//Split this half into 2 halves. If it would become 0, make it 1 instead.
		portion = (portion / 2) == 0 ? 1 : portion / 2;

		//Check which way to go
		if (myInt < myVector[currentPos]) {
			//Go right
			//Add the portion to the current position
			currentPos += portion;
		}
		else if (myInt > myVector[currentPos]) {
			//Go left
			//Subtract the portion from the current position
			currentPos -= portion;
		}
		else {
			std::cout << "Error when checking which way to go" << std::endl;
		}

	}

	for (int i = 0; i < myVector.size(); i++) {
		std::cout << "myVector[" << i << "]= " << myVector[i] << std::endl;
	}
	std::cout << "\n";
}
