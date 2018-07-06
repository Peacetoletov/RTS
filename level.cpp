#include "level.h"
#include "graphics.h"

#include <SDL.h>

#include <math.h>
#include <iostream>

#include "tile.h"		//testing only

using namespace std;

/*
Needs to be modified to work with an RTS.
*/

Level::Level() {}

Level::Level(std::string levelName, int rows, int columns, Graphics* graphicsP) :
		_levelName(levelName)
{
	this->_mapP = new Map(10, 8);
	this->_mapP->loadTestMap();
	this->_mapP->loadTestObject(1, 1);
	this->_pathfinderP = new Pathfinder(this->_mapP, graphicsP);

	//findPath test
	/*
	Tile** tiles = this->_mapP->getTilesP();
	int startRow = 5;
	int startColumn = 5;
	int endRow = 2;
	int endColumn = 2;
	this->_pathfinderP->findPath(tiles[this->_mapP->positionToId(startRow, startColumn)],
		tiles[this->_mapP->positionToId(endRow, endColumn)]);
		*/

	//this->loadMap(mapName, graphics);
}

Level::~Level() {
	delete this->_mapP;
	delete this->_pathfinderP;
	cout << "Destryoing level!" << endl;
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

Pathfinder * Level::getPathfinderP() {
	return this->_pathfinderP;
}

Map * Level::getMapP() {
	return this->_mapP;
}