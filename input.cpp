#include "input.h"

#include <iostream>

/* Input class
Keeps track of keyboard state
*/

//This function gets called at the beginning of each new frame to reset the keys that are no longer relevant
void Input::beginNewFrame() {
	_pressedKeys.clear();
	_releasedKeys.clear();
	_mouseMoved = false;

	_pressedMouseButtons.clear();
	_releasedMouseButtons.clear();
}

//This gets called when a key has been pressed
void Input::keyDownEvent(const SDL_Event& event) {
	_pressedKeys[event.key.keysym.scancode] = true;
	_heldKeys[event.key.keysym.scancode] = true;
}

//This gets called when a key gets released
void Input::keyUpEvent(const SDL_Event& event) {
	_releasedKeys[event.key.keysym.scancode] = true;
	_heldKeys[event.key.keysym.scancode] = false;
}

//This gets called when mouse moves over the window
void Input::mouseMotionEvent(const SDL_Event& event) {
	_mouseMoved = true;
	_mouseX = event.motion.x;
	_mouseY = event.motion.y;
}

//This gets called when a mouse button has been pressed
void Input::mouseButtonDownEvent(const SDL_Event& event) {
	_pressedMouseButtons[event.button.button] = true;
	_heldMouseButtons[event.button.button] = true;
}

//This gets called when a mouse button gets released
void Input::mouseButtonUpEvent(const SDL_Event& event) {
	_releasedMouseButtons[event.button.button] = true;
	_heldMouseButtons[event.button.button] = false;
}

//Check if a certain key was pressed during the current frame
bool Input::wasKeyPressed(SDL_Scancode key) {
	return _pressedKeys[key];
}

//Check if a certain key was released during the current frame
bool Input::wasKeyReleased(SDL_Scancode key) {
	return _releasedKeys[key];
}

//Check if a certain key is currently being held
bool Input::isKeyHeld(SDL_Scancode key) {
	return _heldKeys[key];
}

//Check if a certain mouse button was pressed during the current frame
bool Input::wasMouseButtonPressed(Uint8 button) {
	return _pressedMouseButtons[button];
}

//Check if a certain mouse button was released during the current frame
bool Input::wasMouseButtonReleased(Uint8 button) {
	return _releasedMouseButtons[button];
}

//Check if a certain mouse button is currently being held
bool Input::isMouseButtonHeld(Uint8 button) {
	return _heldMouseButtons[button];
}

bool Input::didMouseMove() {
	return _mouseMoved;
}

int Input::getMouseX() {
	return _mouseX;
}

int Input::getMouseY() {
	return _mouseY;
}