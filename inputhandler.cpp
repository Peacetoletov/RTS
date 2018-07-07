#include "inputhandler.h"
#include "globals.h"		//tileSize

#include <iostream>

//test
#include <vector>

InputHandler::InputHandler() {}

InputHandler::InputHandler(Input* inputP, Level* levelP) :
	_inputP(inputP),
	_levelP(levelP)
{

}

void InputHandler::handleInput() {
	if (_inputP->wasMouseButtonPressed(SDL_BUTTON_LEFT)) {
		//std::cout << "Left mouse button pressed!" << std::endl;
		leftMouseButtonPressed();
	}
	if (_inputP->wasMouseButtonPressed(SDL_BUTTON_RIGHT)) {
		//std::cout << "Right mouse button pressed!" << std::endl;
	}
	if (_inputP->wasKeyPressed(SDL_SCANCODE_ESCAPE)) {
		return;
	}
}

void InputHandler::leftMouseButtonPressed() {
	//Check if mouse if within the borders of the map (terrain)
	int mouseX = this->_inputP->getMouseX();
	int mouseY = this->_inputP->getMouseY();
	if ((mouseX > 0 && mouseX < this->_levelP->getMapP()->getColumns() * globals::TILE_SIZE) &&
		(mouseY > 0 && mouseY < this->_levelP->getMapP()->getRows() * globals::TILE_SIZE)) {
		//std::cout << "Mouse clicked within the borders! Good boy." << std::endl;
		Map* mapP = this->_levelP->getMapP();
		//mapP->getObjectsP[0]->setTargetTileP(mapP->getTilesP[mapP->positionToId(mouseY / globals::TILE_SIZE, mouseX / globals::TILE_SIZE)]);
		
		Tile* targetTileP = mapP->getTilesP()[mapP->positionToId(mouseY / globals::TILE_SIZE, mouseX / globals::TILE_SIZE)];

		//Set the targetTileP of the (only) GameObject in _objects in Map
		(*mapP->getObjectsP())[0]->setTargetTileP(targetTileP);

		//Test pathfinding
		Tile* startTile = mapP->getTilesP()[(*mapP->getObjectsP())[0]->getId()];		//It's some kind of elvish, I can't read it	
		this->_levelP->getPathfinderP()->A_Star(startTile, targetTileP);
	}
}