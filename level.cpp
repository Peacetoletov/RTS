#include "level.h"
#include "graphics.h"
#include "unit.h"
#include "pathfinder.h"

#include <SDL.h>

//#include <math.h>
#include <iostream>

/*
Needs to be modified to work with an RTS.
*/

Level::Level() {}

Level::Level(std::string levelName, int rows, int columns, Graphics* graphicsP, Pathfinder* pathfinderP) :
		_levelName(levelName)
{
	_mapP = new Map(80, 100, pathfinderP);
	_mapP->loadTestMap();

	const int unitsAmount = 4;
	int row[unitsAmount] = { 1, 1, 1, 1 };
	int column[unitsAmount] = { 1, 2, 50, 54 };
	Unit::Type type[unitsAmount] = { Unit::Type::AIR, Unit::Type::AIR, Unit::Type::LAND, Unit::Type::AIR };

	_mapP->loadTestUnits(unitsAmount, row, column, type);
	//this->_pathfinderP = new Pathfinder(this->_mapP, graphicsP);

	//this->loadMap(mapName, graphics);
}

Level::~Level() {
	delete this->_mapP;
	//delete this->_pathfinderP;
	std::cout << "Destryoing level!" << std::endl;
}

/*
void Level::loadMap(std::string mapName, Graphics &graphics) {
	//temporary code to load the background
	this->_backgroundTexture = SDL_CreateTextureFromSurface(graphics.getRenderer(), 
		graphics.loadImage("content/backgrounds/bkBlue.png"));
	this->_size = Vector2(1280, 960);
}
*/

void Level::update(int elapsedTime) {
	//This might cause omtimization problems. If that proves to be the case, I need to redesign it.
	for (int i = 0; i < elapsedTime; i++) {		//Is this a good way of doing this?
		
		//Update map
		_mapP->update();
		
	}
}

void Level::draw(Graphics &graphics) {
	/*
	//Draw the background
	SDL_Rect sourceRect = {0, 0, 64, 64};		//startX, startY, width, height
	SDL_Rect destRect;
	for (int x = 0; x < this->_size.x / 64; x++) {
		for (int y = 0; y < this->_size.y / 64; y++) {
			destRect.x = x * 64;
			destRect.y = y * 64;
			destRect.w = 64;
			destRect.h = 64;
			graphics.blitSurface(this->_backgroundTexture, &sourceRect, &destRect);
		}
	}
	*/
	
}

/*
Pathfinder * Level::getPathfinderP() {
	return this->_pathfinderP;
}
*/

Map * Level::getMapP() {
	return this->_mapP;
}