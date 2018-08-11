#include "inputhandler.h"
#include "input.h"
#include "level.h"
#include "pathfinder.h"
#include "globals.h"		//tileSize
#include "pathparameters.h"
#include "unit.h"
#include "map.h"
#include "tile.h"

#include <vector>
#include <iostream>


InputHandler::InputHandler() {}

InputHandler::InputHandler(Input* inputP, Level* levelP, Pathfinder* pathfinderP) :
	_inputP(inputP),
	_levelP(levelP),
	_pathfinderP(pathfinderP)
{

}

bool InputHandler::handleInput() {
	if (_inputP->wasMouseButtonPressed(SDL_BUTTON_LEFT)) {
		//std::cout << "Left mouse button pressed!" << std::endl;
		leftMouseButtonPressed();
	}
	if (_inputP->wasMouseButtonPressed(SDL_BUTTON_RIGHT)) {
		//std::cout << "Right mouse button pressed!" << std::endl;
	}
	if (_inputP->didMouseMove()) {
		
	}
	if (_inputP->wasKeyPressed(SDL_SCANCODE_ESCAPE)) {
		return true;
	}
	return false;
}

void InputHandler::update() {
	//Check for unit hovering
	std::vector<Unit*>* unitsP = _levelP->getMapP()->getUnitsP();
	int mouseX = _inputP->getMouseX();
	int mouseY = _inputP->getMouseY();
	for (int i = 0; i < unitsP->size(); i++) {
		int tileId = (*unitsP)[i]->getCurrentTileP()->getId();
		if (mouseX > _levelP->getMapP()->idToColumn(tileId) * globals::TILE_SIZE &&
				mouseX <= _levelP->getMapP()->idToColumn(tileId) * globals::TILE_SIZE + globals::TILE_SIZE &&
				mouseY > _levelP->getMapP()->idToRow(tileId) * globals::TILE_SIZE &&
				mouseY <= _levelP->getMapP()->idToRow(tileId) * globals::TILE_SIZE + globals::TILE_SIZE) {
			//TODO: Continue here, first by creating a "hovered" bool variable in Unit
			(*unitsP)[i]->setHovered(true);
		}
		else {
			(*unitsP)[i]->setHovered(false);
		}
	}
}

void InputHandler::leftMouseButtonPressed() {
	//Check if mouse if within the borders of the map (terrain)
	int mouseX = _inputP->getMouseX();
	int mouseY = _inputP->getMouseY();
	Map* mapP = _levelP->getMapP();
	if ((mouseX > 0 && mouseX < mapP->getColumns() * globals::TILE_SIZE) &&
		(mouseY > 0 && mouseY < mapP->getRows() * globals::TILE_SIZE)) {

		//Commented out before I implement unit selection

		/*
		//Test pathfinding
		Tile* targetTileP = mapP->getTilesP()[mapP->positionToId(mouseY / globals::TILE_SIZE, mouseX / globals::TILE_SIZE)];
		
		//Right now, I only test the pathfinding of 1 unit.
		std::vector<Unit*> units;
		units.push_back((*mapP->getUnitsP())[0]);

		//std::cout << "Setting the start and end tiles!" << std::endl;
		PathParameters* parameters = new PathParameters(PathParameters::A_Star, targetTileP, units);
		_pathfinderP->pushPathParameters(parameters);

		//std::cout << "Notifying!" << std::endl;
		_pathfinderP->getCondP()->notify_one();
		*/

	}
}