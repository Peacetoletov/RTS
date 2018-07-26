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
	Level(std::string levelName, int rows, int columns, Graphics* graphicsP);
	~Level();
	void update(int elapsedTime);
	void draw(Graphics &graphics);
	//Pathfinder * getPathfinderP();
	Map * getMapP();

private:
	std::string _levelName;
	SDL_Texture * _backgroundTexture;
	Map * _mapP;
	//Pathfinder * _pathfinderP;

	/* void loadMap
	Loads a map.
	*/
	//void loadMap(std::string mapName, Graphics &graphics);
};

#endif