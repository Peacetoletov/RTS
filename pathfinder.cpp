#include "pathfinder.h"

#include <SDL.h>
#include "graphics.h"
#include "globals.h"

#include <iostream>


/* class Pathfinder
This class deals with pathfinding.
*/

Pathfinder::Pathfinder() {}

Pathfinder::Pathfinder(Map* mapP) :
	_mapP(mapP)
{
	//TODO: Create tiles
}

void Pathfinder::testDrawTiles(float tileSize, Graphics &graphics) {

	SDL_Renderer* renderer = graphics.getRenderer();
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

	int tilesInColumn = this->_mapP->getRows();
	int tilesInRow = this->_mapP->getColumns();

	//DRAW LINES
	//Draw rows
	for (int row = 0; row < (tilesInColumn + 1); row++) {
		SDL_RenderDrawLine(renderer, 0, (row * tileSize), (tilesInRow * tileSize), (row * tileSize));
	}
	//Draw columns
	for (int column = 0; column < (tilesInRow + 1); column++) {
		SDL_RenderDrawLine(renderer, (column * tileSize), 0, (column * tileSize), (tilesInColumn * tileSize));
	}

	//DRAW TERRAIN
	/* Legend
	Tiles accessible by all units are BLACK.
	Tiles accessible by air units are GREY.
	Tiles accessible by no units are WHITE.
	*/

	//TODO: Fix this. Make terrain drawing great again.
	//I'll do this by looping through all tiles and checking their types.
	/*
	for (int column = 0; column < tilesInRow; column++) {
		for (int row = 0; row < tilesInColumn; row++) {

			if ((*terrainP)[row][column] == Map::ALL) {
				continue;
			}
			else if ((*terrainP)[row][column] == Map::AIR) {
				SDL_SetRenderDrawColor(renderer, 127, 127, 127, SDL_ALPHA_OPAQUE);
			}
			else if ((*terrainP)[row][column] == Map::NONE) {
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
			}

			SDL_Rect rect;
			rect.x = column * tileSize;
			rect.y = row * tileSize;
			rect.w = tileSize;
			rect.h = tileSize;
			SDL_RenderFillRect(renderer, &rect);
		}
	}
	*/

	//DRAW UNITS
	/* Legend
	Land units are GREEN.		//for now, all units are land (and green)
	Air units are BLUE.
	*/
	vector<GameObject*> *units = this->_mapP->getObjectsP();
	for (int i = 0; i < units->size(); i++) {
		SDL_Rect rect;
		//rect.x = (*units)[i]->getColumn() * tileSize;
		//rect.y = (*units)[i]->getRow() * tileSize;
		rect.x = this->_mapP->idToColumn((*units)[i]->getId()) * tileSize;
		rect.y = this->_mapP->idToRow((*units)[i]->getId()) * tileSize;
		rect.w = tileSize;
		rect.h = tileSize;
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(renderer, &rect);
	}
	
}

void Pathfinder::findPath(Tile* start, Tile* end) {
	/*
	I will hold all tiles in a map so that I can access individual tiles by their position.
	I will use  std::pair<int, int> as a key, that way I can use 2 values in the key (column, row).
	Example: std::map<std::pair<int,int>, int> myMap;
	Source: https://stackoverflow.com/questions/1112531/what-is-the-best-way-to-use-two-keys-with-a-stdmap
	*/


}

