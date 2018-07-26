#ifndef DRAWER_H
#define DRAWER_H

#include <vector>

class Graphics;
class Unit;
class Map;

/* class Drawer
Draws.
*/

class Drawer {
public:

	Drawer();		//Won't be used

	Drawer(Graphics* graphicsP, Map* mapP);

	void draw();

private:
	Graphics* _graphicsP;
	Map* _mapP;

	void drawBackground();
	void drawUnits();
	void drawTest();
};

#endif