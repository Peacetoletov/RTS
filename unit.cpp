//TODO: This

#include "unit.h"

Unit::Unit() {}

Unit::Unit(Tile* currentTileP, Unit::Type type) :
	_currentTileP(currentTileP),
	_type(type)
{

}

Unit::~Unit() {

}

void Unit::move() {

}

Unit::Type Unit::getType() {
	return _type;
}

Tile* Unit::getCurrentTileP() {
	return _currentTileP;
}

std::stack<Tile*>* Unit::getPathP() {
	return &_path;
}

bool Unit::getWantsToMove() {
	return _wantsToMove;
}

bool Unit::getMoving() {
	return _moving;
}

int Unit::getDistance() {
	return _distance;
}

void Unit::setWantsToMove(bool wantsToMove) {
	_wantsToMove = wantsToMove;
}

void Unit::setMoving(bool moving) {
	_moving = moving;
}

void Unit::setPath(std::stack<Tile*> path) {
	_path = path;
}

void Unit::setDistance(int distance) {
	_distance = distance;
}