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
		leftMouseButtonPressed();
	}
	if (_inputP->wasMouseButtonReleased(SDL_BUTTON_LEFT)) {
		leftMouseButtonReleased();
	}
	if (_inputP->wasMouseButtonPressed(SDL_BUTTON_RIGHT)) {
		rightMouseButtonPressed();
	}
	if (_inputP->didMouseMove()) {
		
	}
	if (_inputP->wasKeyPressed(SDL_SCANCODE_ESCAPE)) {
		return true;
	}
	return false;
}

void InputHandler::leftMouseButtonPressed() {
	_mouseSelectionStartX = _inputP->getMouseX();
	_mouseSelectionStartY = _inputP->getMouseY();
}

void InputHandler::leftMouseButtonReleased() {
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

		//Check if user created a selection rectangle
		int width = abs(mouseX - _mouseSelectionStartX);
		int height = abs(mouseY - _mouseSelectionStartY);
		if (shouldShowSelectionRect(width, height)) {
			//User wants to select multiple units
			//Only land units are selectable in groups

			//Assign values to startX, startY, endX, endY
			int startX, startY, endX, endY;
			if (_mouseSelectionStartX - mouseX > 0) {
				startX = mouseX;
				endX = _mouseSelectionStartX;
			}
			else {
				startX = _mouseSelectionStartX;
				endX = mouseX;
			}
			if (_mouseSelectionStartY - mouseY > 0) {
				startY = mouseY;
				endY = _mouseSelectionStartY;
			}
			else {
				startY = _mouseSelectionStartY;
				endY = mouseY;
			}

			//Calculate start and end columns and rows
			int startColumn = startX / globals::TILE_SIZE;
			int startRow = startY / globals::TILE_SIZE;
			int endColumn = endX / globals::TILE_SIZE;
			int endRow = endY / globals::TILE_SIZE;

			int columnDiff = endColumn - startColumn;
			int rowDiff = endRow - startRow;

			/* Loop through all selected tiles, check if a land unit stands on it. If it does, 
			add it into a vector and set selected = true.
			*/
			std::vector<Unit*> unitGroup;
			Tile** tilesP = mapP->getTilesP();
			for (int column = startColumn; column < endColumn + 1; column++) {
				for (int row = startRow; row < endRow + 1; row++) {
					Unit* unit = tilesP[mapP->positionToId(row, column)]->getLandUnitP();
					if (unit != nullptr) {
						unitGroup.push_back(unit);
						unit->setSelected(true);
					}
				}
			}

			//Set the selected units in Map
			mapP->setSelectedUnits(unitGroup);
		}
		else {
			//User want to select a single unit
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

					//Set the selected units in Map
					std::vector<Unit*> unitGroup;		//group of 1 unit
					unitGroup.push_back((*unitsP)[i]);
					mapP->setSelectedUnits(unitGroup);

					//No need to continue searching through the rest of the units
					break;
				}
			}
		}

	}
}

void InputHandler::rightMouseButtonPressed() {
	/* TODO
	Right now, it's possible to select a group of units and spam right click faster than I can calculate the paths.
	To get around this, I can check the queue of paths waiting to be calculated, and if there are more than x (3?) 
	path calculations in the queue, the request will be ignored.
	Of course, this can only be applied to human-induced requests.
	*/

	//Check if the mouse is within the borders of the map (terrain)
	int mouseX = _inputP->getMouseX();
	int mouseY = _inputP->getMouseY();
	Map* mapP = _levelP->getMapP();
	if ((mouseX > 0 && mouseX < mapP->getColumns() * globals::TILE_SIZE) &&
		(mouseY > 0 && mouseY < mapP->getRows() * globals::TILE_SIZE)) {
		//Test pathfinding
		//Right now, I only test the pathfinding of 1 unit.
		std::vector<Unit*> selectedUnits = *_levelP->getMapP()->getSelectedUnitsP();		

		//Select the target
		if (!selectedUnits.empty()) {
			Map* mapP = _levelP->getMapP();
			int mouseX = _inputP->getMouseX();
			int mouseY = _inputP->getMouseY();
			Tile* targetTileP = mapP->getTilesP()[mapP->positionToId(mouseY / globals::TILE_SIZE, mouseX / globals::TILE_SIZE)];

			//The unit can only start moving if the target tile is available
			bool canMove = targetTileP->isAvailableForPathfinding(selectedUnits[0]->getType());		//unitsToMove[0]->getType() works because all the units in the vector are of the same type

			if (canMove) {
				//Set path of the selected unit(s)
				//If I'm only sending 1 unit, groupId will be -1
				int groupId = -1;
				if (selectedUnits.size() > 1) {
					groupId = _pathfinderP->getIncrementedCurrentGroupId();
				}
				PathParameters* parameters = new PathParameters(targetTileP, selectedUnits, groupId);		//Deletion is handled in Pathfinder
				_pathfinderP->pushPathParameters(parameters);

				//Notify the other thread
				_pathfinderP->getCondP()->notify_one();
			}
		}

	}
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

	//Group unit selection - check if the left mouse button is being held. If not, set _mouseSelectStartX and Y to mouseX and Y
	if (!_inputP->isMouseButtonHeld(SDL_BUTTON_LEFT)) {
		_mouseSelectionStartX = _inputP->getMouseX();
		_mouseSelectionStartY = _inputP->getMouseY();
	}

}

bool InputHandler::shouldShowSelectionRect(int width, int height) {
	//Returns true if one of the sides of the rectangle would be at least minSize
	int minSize = 5;
	return ((width >= minSize) || (height >= minSize));
}

Input* InputHandler::getInputP() {
	return _inputP;
}

int InputHandler::getMouseSelectionStartX() {
	return _mouseSelectionStartX;
}

int InputHandler::getMouseSelectionStartY() {
	return _mouseSelectionStartY;
}
