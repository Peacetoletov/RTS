#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "input.h"
#include "level.h"

class InputHandler {
public:
	InputHandler();		//Won't be used

	/* InputHandler
	Takes Input* inputP and all objects that can be influenced by input as arguments.
	If the amount of objects possibly influenced by input gets too high, I might group
	them all into a struct or a new type of object and pass that as only 1 argument.
	*/
	//InputHandler(Input* inputP, Level* levelP);
	InputHandler(Input* inputP, Pathfinder* pathfinderP);

	void handleInput();

private:
	Input* _inputP;
	//Level* _levelP;
	Pathfinder* _pathfinderP;

	void leftMouseButtonPressed();
};

#endif