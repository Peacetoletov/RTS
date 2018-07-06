#ifndef GAME_H
#define GAME_H

#include "level.h"
#include "pathfinder.h"

class Graphics;

class Game {
public:
	Game();
	~Game();
private:
	void gameLoop();
	void draw(Graphics &graphics);
	void update(int elapsedTime);

	Level* _levelP;
	//Pathfinder _pathfinder;
};

#endif