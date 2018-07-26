#ifndef GAME_H
#define GAME_H

class Graphics;
class Level;
class Pathfinder;
class Drawer;

class Game {
public:
	Game();
	~Game();
private:
	void gameLoop();
	void draw(Graphics &graphics, Drawer drawer);
	void update(int elapsedTime);

	Level* _levelP;
	Pathfinder* _pathfinderP;
};

#endif