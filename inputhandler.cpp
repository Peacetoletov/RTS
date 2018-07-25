#include "inputhandler.h"
#include "globals.h"		//tileSize
#include "pathparameters.h"
#include "unit.h"

#include <vector>
#include <iostream>

//test
#include <chrono>

InputHandler::InputHandler() {}

/*
InputHandler::InputHandler(Input* inputP, Level* levelP) :
	_inputP(inputP),
	_levelP(levelP)
{

}
*/

InputHandler::InputHandler(Input* inputP, Pathfinder* pathfinderP) :
	_inputP(inputP),
	_pathfinderP(pathfinderP)
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
	Map* mapP = _pathfinderP->getMapP();
	if ((mouseX > 0 && mouseX < mapP->getColumns() * globals::TILE_SIZE) &&
		(mouseY > 0 && mouseY < mapP->getRows() * globals::TILE_SIZE)) {

		//Test pathfinding
		Tile* targetTileP = mapP->getTilesP()[mapP->positionToId(mouseY / globals::TILE_SIZE, mouseX / globals::TILE_SIZE)];
		//Tile* startTile = mapP->getTilesP()[(*mapP->getUnitsP())[0]->getId()];		//It's some kind of elvish, I can't read it	
		
		/* Right now, I only test the only pathfinding algorithm that works, that is A*. That's why I only put 1 element
		into the vector. Once I implement group pathfinding using Dijkstra, I will test this with more elements in the vector.
		*/
		std::vector<Unit*> units;
		units.push_back((*mapP->getUnitsP())[0]);

		std::cout << "Setting the start and end tiles!" << std::endl;
		PathParameters* parameters = new PathParameters(PathParameters::A_Star, targetTileP, units);
		_pathfinderP->pushPathParameters(parameters);

		std::cout << "Notifying!" << std::endl;
		_pathfinderP->getCondP()->notify_one();

	}
}