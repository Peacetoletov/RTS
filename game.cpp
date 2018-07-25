
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
}

void Game::gameLoop() {

	Graphics graphics;
	Input input;
	SDL_Event event;
	
	this->_levelP = new Level("level 1", 10, 10, &graphics);	

	//TODO: Create a Pathfinder thread
	/* TODO: Fix the problem that a pathfinder needs a map (that can change with each level) as a constructuor parameter.
	The thread obviously can't be created more than once. This can be easily done by removing the parameters from 
	the constructor and creating a method named initMap in the Pathfinder object.
	*/
	Pathfinder* pathfinderP = new Pathfinder(_levelP->getMapP(), &graphics);
	std::thread pathfinderThread(&Pathfinder::threadStart, pathfinderP);
	pathfinderThread.detach();		//Make the threads run independently
	//TODO: Delete pathfinderP

	//InputHandler inputHandler(&input, this->_levelP);
	InputHandler inputHandler(&input, pathfinderP);

	int test = 0; //test
		
	int LAST_UPDATE_TIME = SDL_GetTicks();
	//Start the game loop
	while (true) {

		if (test % 100 == 0)
			std::cout << "Test int: " << test << std::endl;		//test
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
		inputHandler.handleInput();

		//Some kind of magic
		const int CURRENT_TIME_MS = SDL_GetTicks();
		int ELAPSED_TIME_MS = CURRENT_TIME_MS - LAST_UPDATE_TIME;
		this->update(std::min(ELAPSED_TIME_MS, MAX_FRAME_TIME));
		LAST_UPDATE_TIME = CURRENT_TIME_MS;

		//Draw
		this->draw(graphics);
	}
}


void Game::draw(Graphics &graphics) {
	graphics.clear();

	//this->_level.draw(graphics);
	this->_levelP->getPathfinderP()->testDrawTiles();		

	graphics.flip();
}

void Game::update(int elapsedTime) {
	//this->_level.update(elapsedTime);
}
