#include "inputhandler.h"
#include "globals.h"		//tileSize

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
		//std::cout << "Mouse clicked within the borders! Good boy." << std::endl;
		
		//mapP->getObjectsP[0]->setTargetTileP(mapP->getTilesP[mapP->positionToId(mouseY / globals::TILE_SIZE, mouseX / globals::TILE_SIZE)]);
		
		Tile* targetTileP = mapP->getTilesP()[mapP->positionToId(mouseY / globals::TILE_SIZE, mouseX / globals::TILE_SIZE)];

		//Set the targetTileP of the (only) GameObject in _objects in Map
		(*mapP->getObjectsP())[0]->setTargetTileP(targetTileP);

		//Test pathfinding
		Tile* startTile = mapP->getTilesP()[(*mapP->getObjectsP())[0]->getId()];		//It's some kind of elvish, I can't read it	
		//this->_levelP->getPathfinderP()->A_Star(startTile, targetTileP);

		//Optimization test
		/*
		std::cout << "Starting search. " << std::endl;
		double startTime = time(NULL);

		for (int i = 0; i < 100; i++) {		//testing the efficiency
			this->_levelP->getPathfinderP()->A_Star(startTile, targetTileP);
		}

		double secondsDiff = (time(NULL) - startTime) / 100;
		std::cout << "Search finished." << secondsDiff << std::endl;
		*/

		//std::cout << "Setting the start and end tiles!" << std::endl;
		_pathfinderP->setTiles(startTile, targetTileP);
		//std::cout << "Notifying!" << std::endl;
		_pathfinderP->getCondP()->notify_one();

		/*
		std::cout << "Starting search * 10. " << std::endl;
		auto start = std::chrono::system_clock::now();

		for (int i = 0; i < 10; i++)
			this->_pathfinderP->A_Star(startTile, targetTileP);

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<float> diff = end - start;
		std::cout << "Search finished. " << floor(diff.count() * 1000) << " milliseconds elapsed." << std::endl;
		*/

		//0.2 seconds per 1 run of a* on a 80x100 map when selecting an unreachable goal with analyzed tiles in a vector.
	}
}