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
	this->_font = TTF_OpenFont("arial.ttf", 15);
}

Pathfinder::~Pathfinder() {
	TTF_CloseFont(this->_font);
}

//Deprecated
void Pathfinder::testDrawTiles() {
	/*
	SDL_Renderer* renderer = this->_graphicsP->getRenderer();
	int tileSize = globals::TILE_SIZE;

	int rows = this->_mapP->getRows();
	int columns = this->_mapP->getColumns();
	*/

	//DRAW TERRAIN
	/* Legend
	Tiles accessible by all units are BLACK.
	Tiles accessible by air units are GREY.
	Tiles accessible by no units are WHITE.
	*/

	/*
	Tile** tiles = this->_mapP->getTilesP();
	for (int id = 0; id < (rows * columns); id++) {
		if (tiles[id]->getTerrainType() == Tile::ALL) {
			continue;
		}
		else if (tiles[id]->getTerrainType() == Tile::AIR) {
			SDL_SetRenderDrawColor(renderer, 127, 127, 127, SDL_ALPHA_OPAQUE);
		}
		else if (tiles[id]->getTerrainType() == Tile::NONE) {
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		}

		SDL_Rect rect;
		rect.x = this->_mapP->idToColumn(id) * tileSize;
		rect.y = this->_mapP->idToRow(id) * tileSize;
		rect.w = tileSize;
		rect.h = tileSize;
		SDL_RenderFillRect(renderer, &rect);
	}

	//DRAW LINES
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

	//Draw rows
	for (int row = 0; row < (rows + 1); row++) {
		SDL_RenderDrawLine(renderer, 0, (row * tileSize), (columns * tileSize), (row * tileSize));
	}
	//Draw columns
	for (int column = 0; column < (columns + 1); column++) {
		SDL_RenderDrawLine(renderer, (column * tileSize), 0, (column * tileSize), (rows * tileSize));
	}
	*/

	//DRAW UNITS
	/* Legend
	Land units are GREEN.		//for now, all units are land (and green)
	Air units are BLUE.
	*/
	/*
	std::vector<Unit*> *units = this->_mapP->getUnitsP();
	for (int i = 0; i < units->size(); i++) {
		SDL_Rect rect;
		rect.x = this->_mapP->idToColumn((*units)[i]->getCurrentTileP()->getId()) * tileSize;
		rect.y = this->_mapP->idToRow((*units)[i]->getCurrentTileP()->getId()) * tileSize;
		rect.w = tileSize;
		rect.h = tileSize;
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(renderer, &rect);
	}
	*/
}

std::stack<Tile*> Pathfinder::A_Star(Tile* start, Tile* target, bool canFly) {

	/* HIGH PRIORITY TODO:
	If I select a long, almost straight path (only 1 tile above or below a straight line), it doesn't 
	find the best path.
	*/

	//Create a vector of analyzed tiles
	//Will be used after the path is found to loop through all the analyzed tiles to reset them.
	std::vector<Tile*> analyzedTiles;

	/* Difference between visited and analyzed tiles
	Analyzed tiles haven't been visited yet. They were analyzed as a neighbour to a visited tile.
	Visited tiles have been previously analyzed and were the best candidates to be visited -
	they had the lowest F value out of all the analyzed tiles.
	*/

	//Create a priority queue of open tiles
	/*
	Open tiles are the tiles which are candidates to be visited. 
	Visited tiles are removed from the queue.
	*/
	std::priority_queue<Tile*, std::vector<Tile*>, Comparator> openTiles;

	//The final path will be stored in this stack
	std::stack<Tile*> finalPath;

	//Set up the start tile
	start->setG(0);
	start->setH(start->calculateH(target));

	//Add the start tile openTiles and analyzedTiles
	analyzedTiles.push_back(start);
	openTiles.push(start);

	//Begin the loop
	bool pathFound = false;
	Tile* currentTile = nullptr;

	while (!pathFound) {

		//Choose the most suitable tile to visit
		if (openTiles.size() != 0) {
			currentTile = openTiles.top();
		}
		else {
			//Out of open tiles, path not found
			break;
			//return finalPath;		//This will return an enmpty stack
		}

		//Remove the pointer to the current tile from the openTiles queue, as I'm about to visit the tile
		openTiles.pop();

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
					(*neighbours)[i]->setG(currentTile->getG() + G_increase);

					/*
					std::cout << "Tile " << _mapP->idToRow((*neighbours)[i]->getId()) << "|" <<
					_mapP->idToColumn((*neighbours)[i]->getId()) << " has F " <<
					(*neighbours)[i]->getF() << std::endl;
					*/

					//Set the parent
					//Only if the new G is smaller than the previous G
					(*neighbours)[i]->setParentP(currentTile);

					//std::cout << "Parent of tile " << (*neighbours)[i]->getId() << " is " << currentTile->getId() << std::endl;
				}

				//This can only happen once per tile
				if ((*neighbours)[i]->getH() == INT_MAX) {

					//Set H value
					int H = (*neighbours)[i]->calculateH(_mapP->getTilesP()[target->getId()]);
					(*neighbours)[i]->setH(H);

					if (H == 0) {
						/*
						std::cout << "Found the end! It's " << _mapP->idToRow((*neighbours)[i]->getId()) <<
							"|" << _mapP->idToColumn((*neighbours)[i]->getId()) << std::endl;
							*/
						pathFound = true;
					}

					//Add this tile to the vector and queue of analyzed and open tiles
					analyzedTiles.push_back((*neighbours)[i]);
					openTiles.push((*neighbours)[i]);
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
		
		/*
		std::cout << "Starting search * 10. " << std::endl;
		auto start = std::chrono::system_clock::now();
		*/

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


		/*
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<float> diff = end - start;
		std::cout << "Search finished. " << floor(diff.count() * 1000) << " milliseconds elapsed." << std::endl;
		*/

		delete parameters;
		popPathParameters();
		locker.unlock();
	}
	
	
}

/*
void Pathfinder::setTiles(Tile* startTileP, Tile* targetTileP) {
	std::unique_lock<std::mutex> locker(_mu);
	_startTileP = startTileP;
	_targetTileP = targetTileP;
	locker.unlock();
}
*/

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

/*
Tile* Pathfinder::getStartTileP() {
	std::unique_lock<std::mutex> locker(_mu);
	return _startTileP;
}

Tile* Pathfinder::getTargetTileP() {
	std::unique_lock<std::mutex> locker(_mu);
	return _targetTileP;
}
*/