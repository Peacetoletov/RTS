#ifndef GAME_H
#define GAME_H

class Graphics;
class Level;
class Pathfinder;
class Drawer;
class InputHandler;

class Game {
public:
	Game();
	~Game();

	//Getters
	Level* getLevelP();

private:
	void gameLoop();
	void draw(Graphics &graphics, Drawer drawer);
	void update(int elapsedTime, InputHandler inputHandler);

	Level* _levelP;
	Pathfinder* _pathfinderP;
};

#endif