#include "pathfinder.h"

#include "graphics.h"
#include "globals.h"

#include <iostream>


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
	Visited tiles are removed from the vector.
	*/
	std::vector<Tile*> openTiles;

	//Set up the start tile
	start->setG(0);
	start->setH(start->calculateH(end));

	//Add the start tile openTiles and analyzedTiles
	analyzedTiles.push_back(start);
	openTiles.push_back(start);

	//Set the start tile as the current tile, begin the loop
	Tile* currentTile = start;
	bool pathFound = false;

	while (!pathFound) {

		//TODO: Sort open tiles, choose the most suitable tile to visit


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

			if (!(*neighbours)[i]->getWasVisited() && (*neighbours)[i]->getType() == Tile::TerrainAvailability::ALL) {

				//This can only happen once per tile
				if ((*neighbours)[i]->getH() == INT_MAX) {
					//Set H value
					int H = (*neighbours)[i]->calculateH(this->_mapP->getTilesP()[end->getId()]);
					(*neighbours)[i]->setH(H);

					//Add this tile to the vector of analyzed and open tiles
					analyzedTiles.push_back((*neighbours)[i]);
					openTiles.push_back((*neighbours)[i]);
				}

				//This can happen multiple times per tile
				
				//Set G value
				/*
				I first need to check if the neighbour tile is diagonal or not.
				If it's diagonal, I would add 14 to the current G, otherwise 10.
				I only change the G value if the new value would be smaller than
				the current one.
				*/

				int G_increase = currentTile->isNeighbourDiagonal((*neighbours)[i]) ? 14 : 10;

				if (currentTile->getG() + G_increase < (*neighbours)[i]->getG()) {
					(*neighbours)[i]->setG(currentTile->getG() + G_increase);

					/*
					std::cout << "Tile " << _mapP->idToRow((*neighbours)[i]->getId()) << "|" <<
					_mapP->idToColumn((*neighbours)[i]->getId()) << " has G " <<
					(*neighbours)[i]->getG() << std::endl;
					*/

					//Set the parent
					//Only if the new G is smaller than the previous G
					(*neighbours)[i]->setParentP(currentTile);

					//std::cout << "Parent of tile " << (*neighbours)[i]->getId() << " is " << currentTile->getId() << std::endl;
				}
			}
		}

		/*
		//test
		for (int i = 0; i < analyzedTiles.size(); i++) {
			std::cout << "Tile " << analyzedTiles[i]->getId() << " was analyzed." << std::endl;
		}
		*/

		//TODO: Reset all analyzed tiles


		//TODO: Correctly set pathFound to true
		pathFound = true;
	}

}

