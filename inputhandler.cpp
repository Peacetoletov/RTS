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
		rightMouseButtonPressed();
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
			(*unitsP)[i]->setHovered(true);
		}
		else {
			(*unitsP)[i]->setHovered(false);
		}
	}
}

void InputHandler::leftMouseButtonPressed() {
	//Check if the mouse is within the borders of the map (terrain)
	int mouseX = _inputP->getMouseX();
	int mouseY = _inputP->getMouseY();
	Map* mapP = _levelP->getMapP();
	if ((mouseX > 0 && mouseX < mapP->getColumns() * globals::TILE_SIZE) &&
		(mouseY > 0 && mouseY < mapP->getRows() * globals::TILE_SIZE)) {

		//Deselect all units
		std::vector<Unit*>* unitsP = _levelP->getMapP()->getUnitsP();
		for (int i = 0; i < unitsP->size(); i++) {
			if ((*unitsP)[i]->getSelected()) {
				(*unitsP)[i]->setSelected(false);
			}
		}

		//Check if the mouse clicked on a unit, selecting it
		for (int i = 0; i < unitsP->size(); i++) {
			int tileId = (*unitsP)[i]->getCurrentTileP()->getId();
			/* This is the same check as in InputHandler::update(). Therefore, I could just check the _hovered
			variable of the unit. Problem with that approach is that this function is called BEFORE the update function,
			meaning the mouse click could be 1 frame off. By checking the condition (seemingly unnecessarily) again, 
			I make sure this is frame perfect.
			*/
			if (mouseX > _levelP->getMapP()->idToColumn(tileId) * globals::TILE_SIZE &&
					mouseX <= _levelP->getMapP()->idToColumn(tileId) * globals::TILE_SIZE + globals::TILE_SIZE &&
					mouseY > _levelP->getMapP()->idToRow(tileId) * globals::TILE_SIZE &&
					mouseY <= _levelP->getMapP()->idToRow(tileId) * globals::TILE_SIZE + globals::TILE_SIZE) {
				//Set the current unit as selected
				(*unitsP)[i]->setSelected(true);

				//No need to continue searching through the rest of the units
				break;
			}
		}

	}
}

void InputHandler::rightMouseButtonPressed() {
	//Test pathfinding
	/* TODO: Fix a bug
	If I select the target in a thin wall, the unit will enter the wall.
	*/
	Map* mapP = _levelP->getMapP();
	int mouseX = _inputP->getMouseX();
	int mouseY = _inputP->getMouseY();
	Tile* targetTileP = mapP->getTilesP()[mapP->positionToId(mouseY / globals::TILE_SIZE, mouseX / globals::TILE_SIZE)];

	//Right now, I only test the pathfinding of 1 unit.
	std::vector<Unit*>* unitsP = _levelP->getMapP()->getUnitsP();		//All units
	std::vector<Unit*> unitsToMove;
	for (int i = 0; i < unitsP->size(); i++) {
	if ((*unitsP)[i]->getSelected()) {
		unitsToMove.push_back((*unitsP)[i]);
		}
	}

	//If there are selected units, set their path
	if (!unitsToMove.empty()) {
	PathParameters* parameters = new PathParameters(PathParameters::A_Star, targetTileP, unitsToMove);
	_pathfinderP->pushPathParameters(parameters);

	//Notify the other thread
	_pathfinderP->getCondP()->notify_one();
	}

}