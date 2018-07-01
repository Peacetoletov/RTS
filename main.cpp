/* Main.cpp
The entry point of the program
*/

#include "game.h"

#include <iostream>
#include <Windows.h>

//Debugging version (with console)
int main(int argc, char *args[]) {

	std::cout << "kokot" << std::endl;

	Game game;

	cout << "Program ended! Type 'kokot' to close the console" << endl;
	string y;
	cin >> y;

	return 0;
}

//Final version (without console)
/*
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {+
	Game game;

	return 0;
}
*/