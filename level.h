#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include "map.h"
#include "pathfinder.h"

#include <iostream>

class Graphics;
struct SDL_Texture;

class Level {
public:
	Level();
	Level(std::string levelName, int rows, int columns, Graphics &graphics);
	~Level();
	void update(int elapsedTime);
	void draw(Graphics &graphics);

	//test purpose only
	Pathfinder * getPathfinderP() {
		std::cout << "Returning pointer to the pathfinder. It has position " << &this->_pathfinder << endl;
		return &this->_pathfinder;
	};
	Map getMap() {
		return this->_map;
	};

	void testPathfinderPointer() {
		this->_pathfinder.testPointer();
	};

private:
	std::string _levelName;
	SDL_Texture* _backgroundTexture;
	Map _map;
	Pathfinder _pathfinder;

	/* void loadMap
	Loads a map.
	*/
	//void loadMap(std::string mapName, Graphics &graphics);
};

#endif