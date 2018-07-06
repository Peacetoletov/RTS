
#include <SDL_image.h>

#include "graphics.h"
#include "globals.h"

#include <iostream>

/* Graphics class
Holds all information dealing with graphics for the game
*/

Graphics::Graphics() {
	SDL_CreateWindowAndRenderer(globals::SCREEN_WIDTH, globals::SCREEN_HEIGHT, 0, &this->_window, &this->_renderer);
	SDL_SetWindowTitle(this->_window, "RTS");
}

Graphics::~Graphics() {
	SDL_DestroyWindow(this->_window);
	SDL_DestroyRenderer(this->_renderer);
	SDL_Quit();
}

SDL_Surface* Graphics::loadImage(const std::string &filePath) {
	//Check if the image hasn't been loaded yet
	if (this->_spriteSheets.count(filePath) == 0) {
		this->_spriteSheets[filePath] = IMG_Load(filePath.c_str());
	}
	return this->_spriteSheets[filePath];
}

void Graphics::blitSurface(SDL_Texture* texture, SDL_Rect* sourceRectangle, SDL_Rect* destinationRectangle) {
	SDL_RenderCopy(this->_renderer, texture, sourceRectangle, destinationRectangle);
}

void Graphics::flip() {
	SDL_RenderPresent(this->_renderer);
}

void Graphics::clear() {
	SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);		//makes the background color black
	SDL_RenderClear(this->_renderer);
}

void Graphics::drawText(std::string text, int x, int y, TTF_Font* font, SDL_Color &color) {
	/* Source of this madness
	http://programmersranch.blogspot.com/2014/03/sdl2-displaying-text-with-sdlttf.html
	http://gigi.nullneuron.net/gigilabs/displaying-text-in-sdl2-with-sdl_ttf/
	*/

	/*
	The current version is quite inefficient. I want to draw a static, non-changing text.
	And I create a new surface, texture and rectangle every frame for each text I draw.
	If I want to draw static text more efficiently, I should only create the surface,
	texture and rectangle once.
	*/

	/* Example of creating *font and &color
	SDL_Color color = { 200, 200, 200 };
	TTF_Font* font = TTF_OpenFont("arial.ttf", 15);
	*/

	//const char* error = TTF_GetError();		//Not sure what this does, but it was in the tutorial
	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(this->_renderer, surface);

	
	//This also has to be like this apparently ¯\_(ツ)_/¯
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = { x, y, texW, texH };

	//This is the method that actually draws the text
	SDL_RenderCopy(this->_renderer, texture, NULL, &dstrect);

	//std::cout << rand() << std::endl;

	//Free the allocated memory
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
}

SDL_Renderer* Graphics::getRenderer() const {
	return this->_renderer;
}