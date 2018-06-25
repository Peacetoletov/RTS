
#include <SDL.h>
#include <algorithm>﻿		//std::min

#include "game.h"
#include "graphics.h"
#include "input.h"
#include "globals.h"		//TILE_SIZE

#include <iostream>
#include <vector>


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
	this->gameLoop();
}

void Game::gameLoop() {
	Graphics graphics;
	Input input;
	SDL_Event event;

	this->_level = Level("level 1", 10, 10, graphics);
	std::cout << "Testing from game.cpp using testPathfinderPointer function" << std::endl;
	this->_level.testPathfinderPointer();		//this is 0

	this->_level.getPathfinder().testPointer();	//this is 0

	std::cout << "Size from game.cpp = " << this->_level.getMap().getTerrain().size() << std::endl;
		
	int LAST_UPDATE_TIME = SDL_GetTicks();
	//Start the game loop
	while (true) {
		input.beginNewFrame();

		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				if (event.key.repeat == 0) {
					input.keyDownEvent(event);
				}
			}
			else if (event.type == SDL_KEYUP) {
				input.keyUpEvent(event);
			}

			else if (event.type == SDL_QUIT) {
				return;
			}
		}
		
		if (input.wasKeyPressed(SDL_SCANCODE_ESCAPE)) {
			return;
		} 

		
		////else if (input.isKeyHeld(SDL_SCANCODE_LEFT)) {
		////	this->_player.moveLeft();
		////} else if (input.isKeyHeld(SDL_SCANCODE_RIGHT)) {
		////	this->_player.moveRight();
		////}

		////if (!input.isKeyHeld(SDL_SCANCODE_LEFT) && !input.isKeyHeld(SDL_SCANCODE_RIGHT)) {
		////	this->_player.stopMoving();
		////}
		

		const int CURRENT_TIME_MS = SDL_GetTicks();
		int ELAPSED_TIME_MS = CURRENT_TIME_MS - LAST_UPDATE_TIME;
		this->update(std::min(ELAPSED_TIME_MS, MAX_FRAME_TIME));
		LAST_UPDATE_TIME = CURRENT_TIME_MS;

		this->draw(graphics);
	}
}


void Game::draw(Graphics &graphics) {
	graphics.clear();

	//this->_level.draw(graphics);
	this->_pathfinder.testDrawTiles(globals::TILE_SIZE, graphics);

	graphics.flip();
}

void Game::update(float elapsedTime) {
	this->_level.update(elapsedTime);
}
