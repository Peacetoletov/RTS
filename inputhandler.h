#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

class Input;
class Level;
class Pathfinder;
struct Rect;

class InputHandler {
public:
	InputHandler();		//Won't be used

	/* InputHandler
	Takes Input* inputP and all objects that can be influenced by input as arguments.
	If the amount of objects possibly influenced by input gets too high, I might group
	them all into a struct or a new type of object and pass that as only 1 argument.
	*/
	//InputHandler(Input* inputP, Level* levelP);
	InputHandler(Input* inputP, Level* levelP, Pathfinder* pathfinderP);

	//The return value determines whether the program should ends
	bool handleInput();

	//This update function covers everything that requires mouse position
	void update();

	//Getters
	Input* getInputP();
	int getMouseSelectStartX();
	int getMouseSelectStartY();

private:
	Input* _inputP;
	Level* _levelP;
	Pathfinder* _pathfinderP;

	int _mouseSelectStartX;
	int _mouseSelectStartY;

	void leftMouseButtonPressed();
	void leftMouseButtonReleased();
	void rightMouseButtonPressed();
};

#endif