
#include <SDL_image.h>

#include "graphics.h"
#include "globals.h"

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

void Graphics::drawText(std::string fontName, int fontSize, SDL_Color color, std::string text, int x, int y) {
	/* Source of this madness
	http://programmersranch.blogspot.com/2014/03/sdl2-displaying-text-with-sdlttf.html
	http://gigi.nullneuron.net/gigilabs/displaying-text-in-sdl2-with-sdl_ttf/
	*/
	TTF_Font* font = TTF_OpenFont(fontName.c_str(), fontSize);
	const char* error = TTF_GetError();		//Not sure what this does, but it was in the tutorial
	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(this->_renderer, surface);

	//This also has to be like this apparently ¯\_(ツ)_/¯
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = { x, y, texW, texH };

	//This is the method that actually draws the text
	SDL_RenderCopy(this->_renderer, texture, NULL, &dstrect);

	//Free the allocated memory
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
	TTF_CloseFont(font);
}

SDL_Renderer* Graphics::getRenderer() const {
	return this->_renderer;
}