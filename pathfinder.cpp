#include "pathfinder.h"

#include <SDL.h>
#include "graphics.h"
#include "globals.h"

#include <iostream>


/* class Pathfinder
This class deals with pathfinding.
*/

Pathfinder::Pathfinder() {}

Pathfinder::Pathfinder(Map* mapp) :
	_mapp(mapp)
{
	
}

void Pathfinder::testDrawTiles(float tileSize, Graphics &graphics) {

	SDL_Renderer* renderer = graphics.getRenderer();
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

	vector<vector<Map::TerrainAvailability> >* terrainP = this->_mapp->getTerrainP();
	int tilesInColumn = terrainP->size();
	int tilesInRow = (*terrainP)[0].size();

	//Draw rows
	for (int row = 0; row < (tilesInColumn + 1); row++) {
		SDL_RenderDrawLine(renderer, 0, (row * tileSize), (tilesInRow * tileSize), (row * tileSize));
	}
	//Draw columns
	for (int column = 0; column < (tilesInRow + 1); column++) {
		SDL_RenderDrawLine(renderer, (column * tileSize), 0, (column * tileSize), (tilesInColumn * tileSize));
	}

	/* Legend
	Tiles accessible by all units are BLACK.
	Tiles accessible by air units are GREY.
	Tiles accessible by no units are WHITE.
	Land units are GREEN.
	Air units are BLUE.
	*/
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
	
}

void Pathfinder::testPointer() {
	std::cout << "Size = " << this->_mapp->getTerrainP()->size() << std::endl;
}