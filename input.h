#ifndef INPUT_H
#define INPUT_H

#include <SDL.h>
#include <map>

class Input {
public:
	void beginNewFrame();

	void keyDownEvent(const SDL_Event& event);
	void keyUpEvent(const SDL_Event& event);

	void mouseMotionEvent(const SDL_Event& event);
	void mouseButtonDownEvent(const SDL_Event& event);
	void mouseButtonUpEvent(const SDL_Event& event);

	bool wasKeyPressed(SDL_Scancode key);
	bool wasKeyReleased(SDL_Scancode key);
	bool isKeyHeld(SDL_Scancode key);

	bool wasMouseButtonPressed(Uint8 button);
	bool wasMouseButtonReleased(Uint8 button);
	bool isMouseButtonHeld(Uint8 button);

	//Getters
	int getMouseX();
	int getMouseY();

private:
	std::map<SDL_Scancode, bool> _heldKeys;
	std::map<SDL_Scancode, bool> _pressedKeys;
	std::map<SDL_Scancode, bool> _releasedKeys;

	std::map<Uint8, bool> _heldMouseButtons;
	std::map<Uint8, bool> _pressedMouseButtons;
	std::map<Uint8, bool> _releasedMouseButtons;
	
	int _mouseX;
	int _mouseY;
};

#endif
