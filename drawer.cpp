#include "drawer.h"
#include "unit.h"
#include "graphics.h"
#include "globals.h"		//tileSize
#include "tile.h"
#include "map.h"
#include "inputhandler.h"
#include "input.h"

#include <iostream>

Drawer::Drawer() {}

Drawer::Drawer(Graphics* graphicsP, Map* mapP, InputHandler* inputHandlerP) :
	_graphicsP(graphicsP),
	_mapP(mapP),
	_inputHandlerP(inputHandlerP)
{

}

void Drawer::draw() {
	//Draw background
	drawBackground();

	//Draw units
	drawUnits();

	//Draw mouse related stuff (cursor, unit selection)
	drawMouse();

	//Draw test
	//drawTest();
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
		if (tiles[id]->getTerrainType() == Tile::TerrainAvailability::ALL) {
			continue;
		}
		else if (tiles[id]->getTerrainType() == Tile::TerrainAvailability::AIR) {
			SDL_SetRenderDrawColor(renderer, 150, 150, 150, SDL_ALPHA_OPAQUE);
		}
		else if (tiles[id]->getTerrainType() == Tile::TerrainAvailability::NONE) {
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
		if ((*units)[i]->getType() == Unit::Type::LAND) {
			if ((*units)[i]->getHovered() || (*units)[i]->getSelected()) {
				SDL_SetRenderDrawColor(renderer, 0, 150, 0, SDL_ALPHA_OPAQUE);
			}
			else {
				SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
			}
		}
		else {
			if ((*units)[i]->getHovered() || (*units)[i]->getSelected()) {
				SDL_SetRenderDrawColor(renderer, 0, 100, 150, SDL_ALPHA_OPAQUE);
			}
			else {
				SDL_SetRenderDrawColor(renderer, 50, 150, 255, SDL_ALPHA_OPAQUE);
			}
		}		
		SDL_RenderFillRect(renderer, &rect);
	}
}

void Drawer::drawMouse() {
	Input* inputP = _inputHandlerP->getInputP();
	int mouseX = inputP->getMouseX();
	int mouseSelectionStartX = _inputHandlerP->getMouseSelectionStartX();
	int mouseY = inputP->getMouseY();
	int mouseSelectionStartY = _inputHandlerP->getMouseSelectionStartY();
	int width = abs(mouseX - mouseSelectionStartX);
	int height = abs(mouseY - mouseSelectionStartY);

	if (_inputHandlerP->shouldShowSelectionRect(width, height)) {
		SDL_Renderer* renderer = this->_graphicsP->getRenderer();
		SDL_Rect rect;
		rect.x = (mouseSelectionStartX - mouseX > 0) ? mouseX : mouseSelectionStartX;
		rect.y = (mouseSelectionStartY - mouseY > 0) ? mouseY : mouseSelectionStartY;
		rect.w = width;
		rect.h = height;
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);		//allows the use of alpha channel
		SDL_SetRenderDrawColor(renderer, 64, 224, 208, 150);
		SDL_RenderFillRect(renderer, &rect);

	}
}

void Drawer::drawTest() {
	TTF_Font* font = TTF_OpenFont("arial.ttf", 10);
	SDL_Color color = { 200, 200, 200 };

	for (int i = 0; i < _mapP->getColumns() * _mapP->getRows(); i++) {
		Tile* tile = _mapP->getTilesP()[i];
		if (tile->getG() != INT_MAX) {
			//This tile was analyzed
			int F = tile->getF();
			int x = _mapP->idToColumn(tile->getId()) * globals::TILE_SIZE + 5;
			int y = _mapP->idToRow(tile->getId()) * globals::TILE_SIZE + 5;

			_graphicsP->drawText(std::to_string(F), x, y, font, color);
		}
	}
}