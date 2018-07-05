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
	//TODO: Create tiles
}

void Pathfinder::testDrawTiles(float tileSize) {

	SDL_Renderer* renderer = this->_graphicsP->getRenderer();

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
	vector<GameObject*> *units = this->_mapP->getObjectsP();
	for (int i = 0; i < units->size(); i++) {
		SDL_Rect rect;
		rect.x = this->_mapP->idToColumn((*units)[i]->getId()) * tileSize;
		rect.y = this->_mapP->idToRow((*units)[i]->getId()) * tileSize;
		rect.w = tileSize;
		rect.h = tileSize;
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(renderer, &rect);
	}

	SDL_Color color = { 255, 255, 0 };
	_graphicsP->drawText("arial.ttf", 15, color, "Hello, programmer!", 0, 400);
	
}

void Pathfinder::findPath(Tile* start, Tile* end) {
	
	//Set up the start tile
	start->setG(0);
	start->setH(start->calculateH(end));
	
	//Set the start tile as the current tile, begin the loop
	Tile* currentTile = start;
	bool pathFound = false;

	while (!pathFound) {
		//ANALYZE NEIGHBOURS


		//MARK THE CURRENT TILE OFF OF THE LIST
		currentTile->setWasChecked(true);

		/*
		Maybe create a vector of checked tiles?
		*/

		pathFound = true;
	}

}

