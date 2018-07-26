/* Main.cpp
The entry point of the program
*/

#include "game.h"
#include "test.h"

#include <iostream>		
#include <Windows.h>
#include <string>

//Detecting memory leaks
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//Debugging version (with console)
int main(int argc, char *args[]) {
	
	//std::cout << "kokot" << std::endl;
	
	//Test test;			//1 of these 2 must be commented out
	Game game;				//1 of these 2 must be commented out

	_CrtDumpMemoryLeaks();	//Needs to be before the cin because otherwise it would report memory leaks for some reason

	std::cout << "Program ended! Type 'kokot' to close the console" << std::endl;
	std::string y;
	std::cin >> y;
	
	return 0;

}

//Final version (without console)
/*
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {+
	Game game;

	return 0;
}
*/