
#include <SDL.h>
#include <algorithm>﻿		//std::min
#include <SDL_ttf.h>		//fonts, drwaing text

#include "game.h"
#include "graphics.h"
#include "input.h"
#include "inputhandler.h"
#include "globals.h"		//TILE_SIZE

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

	/*
	cout << "The value of tile [0][0] is " << (*this->_level->getMapP()->getTerrainP())[0][0] << endl;
	this->_level->getMapP()->setTerrainTile(0, 0, Map::NONE);
	cout << "The value of tile [0][0] after modifying it is " << (*this->_level->getMapP()->getTerrainP())[0][0] << endl;
	*/

	InputHandler inputHandler(&input, this->_levelP);
		
	int LAST_UPDATE_TIME = SDL_GetTicks();
	//Start the game loop
	while (true) {
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
