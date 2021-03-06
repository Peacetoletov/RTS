#ifndef DRAWER_H
#define DRAWER_H

#include <vector>

class Graphics;
class Unit;
class Map;
class InputHandler;

/* class Drawer
Draws.
*/

class Drawer {
public:

	Drawer();		//Won't be used

	Drawer(Graphics* graphicsP, Map* mapP, InputHandler* inputHandlerP);

	void draw();

private:
	Graphics* _graphicsP;
	Map* _mapP;
	InputHandler* _inputHandlerP;

	void drawBackground();
	void drawUnits();
	void drawMouse();
	bool shouldDrawSelectionRect(int minSize, int width, int height);

	void drawTest();
};

#endif