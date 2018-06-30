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

	//std::cout << "Size of vector in pathfinder's constructor using the original pointer = " << mapp->getTerrain().size() << endl;

	//std::cout << "Size of vector in pathfinder's constructor using this + original pointer = " << this->_mapp->getTerrain().size() << endl;

	std::cout << "Creating pathfinder" << endl;

	//std::cout << "Value of _mapp in constructor = " << _mapp << endl;

	testPointer();
}

void Pathfinder::testDrawTiles(float tileSize, Graphics &graphics) {

	SDL_Renderer* renderer = graphics.getRenderer();
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

	//Draw rows
	//cout << "ceil(globals::SCREEN_HEIGHT / tileSize) = " << ceil(globals::SCREEN_HEIGHT / tileSize) << endl;
	for (int row = 0; row < ceil(globals::SCREEN_HEIGHT / tileSize); row++) {
	SDL_RenderDrawLine(renderer, 0, (row * tileSize), globals::SCREEN_WIDTH, (row * tileSize));
	}
	//Draw columns
	for (int column = 0; column < floor(globals::SCREEN_WIDTH / tileSize); column++) {
	SDL_RenderDrawLine(renderer, (column * tileSize), 0, (column * tileSize), globals::SCREEN_HEIGHT);
	}
}

void Pathfinder::testPointer() {
	//vector<vector<TerrainAvailability> > terrain = this->_mapp->getTerrain();

	//std::cout << "Inside testPointer function; this->_mapp = " << this->_mapp << endl;

	std::cout << "Size = " << this->_mapp->getTerrainP()->size() << std::endl;
	//std::cout << "Size = " << this->_mapp->getTerrain() << std::endl;
}