#include "drawer.h"
#include "unit.h"
#include "graphics.h"
#include "globals.h"		//tileSize
#include "map.h"

Drawer::Drawer() {}

Drawer::Drawer(Graphics* graphicsP, Map* mapP) :
	_graphicsP(graphicsP),
	_mapP(mapP)
{

}

void Drawer::draw() {
	//Draw background
	drawBackground();

	//Draw units
	drawUnits();
}

void Drawer::drawBackground() {
	SDL_Renderer* renderer = this->_graphicsP->getRenderer();
	int tileSize = globals::TILE_SIZE;

	int rows = _mapP->getRows();
	int columns = _mapP->getColumns();

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
}

void Drawer::drawUnits() {
	SDL_Renderer* renderer = this->_graphicsP->getRenderer();
	int tileSize = globals::TILE_SIZE;

	int rows = _mapP->getRows();
	int columns = _mapP->getColumns();

	/* Legend
	Land units are GREEN.		//for now, all units are land (and green)
	Air units are BLUE.
	*/
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
}