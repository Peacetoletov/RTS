#include "input.h"

#include <iostream>

/* Input class
Keeps track of keyboard state
*/

//This function gets called at the beginning of each new frame to reset the keys that are no longer relevant
void Input::beginNewFrame() {
	this->_pressedKeys.clear();
	this->_releasedKeys.clear();

	this->_pressedMouseButtons.clear();
	this->_releasedMouseButtons.clear();
}

//This gets called when a key has been pressed
void Input::keyDownEvent(const SDL_Event& event) {
	this->_pressedKeys[event.key.keysym.scancode] = true;
	this->_heldKeys[event.key.keysym.scancode] = true;
}

//This gets called when a key gets released
void Input::keyUpEvent(const SDL_Event& event) {
	this->_releasedKeys[event.key.keysym.scancode] = true;
	this->_heldKeys[event.key.keysym.scancode] = false;
}

//This gets called when mouse moves over the window
void Input::mouseMotionEvent(const SDL_Event& event) {
	this->_mouseX = event.motion.x;
	this->_mouseY = event.motion.y;
}

//This gets called when a mouse button has been pressed
void Input::mouseButtonDownEvent(const SDL_Event& event) {
	this->_pressedMouseButtons[event.button.button] = true;
	this->_heldMouseButtons[event.button.button] = true;
}

//This gets called when a mouse button gets released
void Input::mouseButtonUpEvent(const SDL_Event& event) {
	this->_releasedMouseButtons[event.button.button] = true;
	this->_heldMouseButtons[event.button.button] = false;
}

//Check if a certain key was pressed during the current frame
bool Input::wasKeyPressed(SDL_Scancode key) {
	return this->_pressedKeys[key];
}

//Check if a certain key was released during the current frame
bool Input::wasKeyReleased(SDL_Scancode key) {
	return this->_releasedKeys[key];
}

//Check if a certain key is currently being held
bool Input::isKeyHeld(SDL_Scancode key) {
	return this->_heldKeys[key];
}

//Check if a certain mouse button was pressed during the current frame
bool Input::wasMouseButtonPressed(Uint8 button) {
	return this->_pressedMouseButtons[button];
}

//Check if a certain mouse button was released during the current frame
bool Input::wasMouseButtonReleased(Uint8 button) {
	return this->_releasedMouseButtons[button];
}

//Check if a certain mouse button is currently being held
bool Input::isMouseButtonHeld(Uint8 button) {
	return this->_heldMouseButtons[button];
}

int Input::getMouseX() {
	return this->_mouseX;
}

int Input::getMouseY() {
	return this->_mouseY;
}