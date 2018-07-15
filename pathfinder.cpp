#include "pathfinder.h"

#include "graphics.h"
#include "globals.h"
#include "comparator.h"
#include <algorithm>		//std::sort
#include <queue>			//std::priority_queue


#include <iostream>

//test
#include <time.h>


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

void Pathfinder::testDrawTiles() {

	SDL_Renderer* renderer = this->_graphicsP->getRenderer();
	int tileSize = globals::TILE_SIZE;

	int rows = this->_mapP->getRows();
	int columns = this->_mapP->getColumns();

	//DRAW TERRAIN
	/* Legend
	Tiles accessible by all units are BLACK.
	Tiles accessible by air units are GREY.
	Tiles accessible by no units are WHITE.
	*/

	Tile** tiles = this->_mapP->getTilesP();
	for (int id = 0; id < (rows * columns); id++) {
		if (tiles[id]->getType() == Tile::ALL) {
			continue;
		}
		else if (tiles[id]->getType() == Tile::AIR) {
			SDL_SetRenderDrawColor(renderer, 127, 127, 127, SDL_ALPHA_OPAQUE);
		}
		else if (tiles[id]->getType() == Tile::NONE) {
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

	//DRAW UNITS
	/* Legend
	Land units are GREEN.		//for now, all units are land (and green)
	Air units are BLUE.
	*/
	std::vector<GameObject*> *units = this->_mapP->getObjectsP();
	for (int i = 0; i < units->size(); i++) {
		SDL_Rect rect;
		rect.x = this->_mapP->idToColumn((*units)[i]->getId()) * tileSize;
		rect.y = this->_mapP->idToRow((*units)[i]->getId()) * tileSize;
		rect.w = tileSize;
		rect.h = tileSize;
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(renderer, &rect);
	}

	//TEST: Draw the H value for each tile, end tile is 4|1
	/*
	SDL_Color color = { 200, 200, 200 };
	
	for (int id = 0; id < (rows * columns); id++) {
		int targetTileId = (*this->_mapP->getObjectsP())[0]->getTargetTileP()->getId();
		tiles[id]->setH(tiles[id]->calculateH(tiles[targetTileId]));		//sets H
		int row = this->_mapP->idToRow(id);
		int column = this->_mapP->idToColumn(id);
		int x = 3 + column * tileSize;
		int y = (tileSize - 20) + row * tileSize;
		_graphicsP->drawText(std::to_string(tiles[id]->getH()), x, y, this->_font, color);
	}
	*/
}

void Pathfinder::A_Star(Tile* start, Tile* end) {
	/* TODO
	Change arguments from (Tile* start, Tile* end) to (GameObject* unit, Tile* end).
	That will allow for more precise checking - right now, the pathfinder will consider both types of
	obstacles as unpassable. Instead, it should be allowed to move through tiles accessible by air units,
	if the unit is air-type.
	The reason why I'm not doing it yet is that this version is easier to test, as it doesn't rely
	on any units.
	*/

	/* TODO
	Put all the tiles that are included in the path to a vector and pass the vector to the unit.
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

	//Set up the start tile
	start->setG(0);
	start->setH(start->calculateH(end));

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
			//std::cout << "Out of tiles! Path not found!" << std::endl;
			break;
		}

		/*
		//Test
		std::cout << "Visiting tile " << _mapP->idToRow(currentTile->getId()) <<
			"|" << _mapP->idToColumn(currentTile->getId()) << ". It has F " << 
			currentTile->getF() << ". Amount of tiles in openTiles: " << 
			openTiles.size() << std::endl;
			*/
		

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

			if (!(*neighbours)[i]->getWasVisited() && (*neighbours)[i]->getType() == Tile::TerrainAvailability::ALL) {

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
					int H = (*neighbours)[i]->calculateH(this->_mapP->getTilesP()[end->getId()]);
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

	//Reset all analyzed tiles
	for (int i = 0; i < analyzedTiles.size(); i++) {
		analyzedTiles[i]->reset();
	}
}
