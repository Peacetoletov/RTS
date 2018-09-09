
#include <SDL.h>
#include <algorithm>﻿		//std::min
#include <SDL_ttf.h>		//fonts, drwaing text
#include <thread>			//std::thread

#include "game.h"
#include "graphics.h"
#include "input.h"
#include "inputhandler.h"
#include "globals.h"		//TILE_SIZE
#include "pathfinder.h"
#include "level.h"
#include "drawer.h"

#include <iostream>

/* Game class
This class holds information for the main game loop
*/

namespace {
	const int FPS = 50;
	const int MAX_FRAME_TIME = 5 * 1000 / FPS;
}

Game::~Game() {}

Game::Game() {
	SDL_Init(SDL_INIT_EVERYTHING);		
	TTF_Init();							
	this->gameLoop();					

	delete this->_levelP;	
	delete this->_pathfinderP;
}

void Game::gameLoop() {

	Graphics graphics;
	Input input;
	SDL_Event event;

	/* The thread pathfinderThread is causing a minor memory leak (8 bytes) because of the function it uses.
	There's an infinite loop in it and that causes a memory leak for some reason. When I commented the loop out,
	the leak disappeared. It's small enough that I don't need to worry about it.
	*/
	_pathfinderP = new Pathfinder();
	std::thread pathfinderThread(&Pathfinder::threadStart, _pathfinderP);		//Memory leak here
	pathfinderThread.detach();		//Make the threads run independently
	
	_levelP = new Level("level 1", 10, 10, &graphics, _pathfinderP);	

	_pathfinderP->initMap(_levelP->getMapP());

	InputHandler inputHandler(&input, _levelP, _pathfinderP);		
	Drawer drawer(&graphics, _levelP->getMapP(), &inputHandler);		//Needs to be created after the level and inputhandler

	int test = 0; //test
		
	int LAST_UPDATE_TIME = SDL_GetTicks();
	//Start the game loop
	while (true) {

		if (test % 100 == 0)
			//std::cout << "Test int: " << test << std::endl;		//test
		test++;

		//Register input
		input.beginNewFrame();
		if (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.repeat == 0) {
					input.keyDownEvent(event);
				}
				break;
			case SDL_KEYUP:
				input.keyUpEvent(event);
				break;
			case SDL_MOUSEMOTION:
				input.mouseMotionEvent(event);
				break;
			case SDL_MOUSEBUTTONDOWN:
				input.mouseButtonDownEvent(event);
				break;
			case SDL_MOUSEBUTTONUP:
				input.mouseButtonUpEvent(event);
				break;
			case SDL_QUIT:
				return;
				break;
			}
		}

		//Handle the input
		bool quit = inputHandler.handleInput();
		if (quit) {
			return;
		}

		//Some kind of magic
		const int CURRENT_TIME_MS = SDL_GetTicks();
		int ELAPSED_TIME_MS = CURRENT_TIME_MS - LAST_UPDATE_TIME;
		this->update(std::min(ELAPSED_TIME_MS, MAX_FRAME_TIME), inputHandler);
		LAST_UPDATE_TIME = CURRENT_TIME_MS;

		//Draw
		this->draw(graphics, drawer);
	}
}

void Game::draw(Graphics &graphics, Drawer drawer) {
	graphics.clear();

	drawer.draw();

	graphics.flip();
}

void Game::update(int elapsedTime, InputHandler& inputHandler) {
	inputHandler.update();
	_levelP->update(elapsedTime);		
}

Level* Game::getLevelP() {
	return _levelP;
}