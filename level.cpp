#include "level.h"
#include "graphics.h"

#include <SDL.h>

#include <math.h>
#include <iostream>

using namespace std;

/*
Needs to be modified to work with an RTS.
*/

Level::Level() {}

Level::Level(std::string levelName, int rows, int columns, Graphics &graphics) :
		_levelName(levelName)
{
	this->_map = Map(rows, columns);

	cout << "Size of vector in level = " << this->_map.getTerrain().size() << endl;

	this->_pathfinder = Pathfinder(&_map);

	cout << "Testing the function from level" << endl;
	this->_pathfinder.testPointer();

	//this->loadMap(mapName, graphics);
}

Level::~Level() {}

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

