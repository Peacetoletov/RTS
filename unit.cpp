#include "unit.h"
#include "tile.h"
#include "globals.h"

#include <iostream>

Unit::Unit() {}

Unit::Unit(Tile* currentTileP, Unit::Type type, std::vector<Unit*>* unitsP) :
	_currentTileP(currentTileP),
	_type(type),
	_unitsP(unitsP)
{

}

Unit::~Unit() {

}

void Unit::update() {
	/* If the unit wants to move and isn't moving yet, set moving to true (if it's possible) and
	calculate the distance it needs to travel (diagonal distance is longer).
	*/
	//std::cout << "Want to move? " << _wantsToMove << std::endl;

	/*
	if (_path.size() == 0) {
		return;
	}*/

	if (_wantsToMove && !_moving) {

		//Check whether the next tile is occupied by a unit of the same type
		if (!_path.top()->isAvailable(_type)) {

			//Desired tile is occupied, unit cannot move.
			//_moving = false;		//unnecessary
			//std::cout << "Next tile is occupied!" << std::endl;

			/* When 2 units are moving towards each other from opposite directions, they would get stuck.
			This makes one unit get out of the way, let the other unit pass and then move back and continue.
			*/
			avoidOppositeUnit();
		}
		else {
			//Desired tile isn't occupied, unit can start moving.
			_moving = true;

			//Set distance
			_distance = _currentTileP->isNeighbourDiagonal(_path.top()) ?
				globals::TILE_DIAGONAL_DISTANCE : globals::TILE_STRAIGHT_DISTANCE;

			//Set pointers to this unit in the current tile and the next tile
			setPointersToThisUnit();

			//Set the current tile to the tile the unit is moving onto
			_currentTileP = _path.top();

			//Remove the tile from the stack
			_path.pop();
			
			if (_path.size() == 0) {
				_wantsToMove = false;
				_moving = false;
			}
			
		}
	}

	//Move the unit
	if (_moving) {
		move();
	}
	
}

void Unit::avoidOppositeUnit() {
	//Select the unit that's in the way
	Unit* oppositeUnit = nullptr;
	for (int i = 0; i < _unitsP->size(); i++) {
		if ((*_unitsP)[i]->getCurrentTileP()->getId() == _path.top()->getId()) {
			oppositeUnit = (*_unitsP)[i];
			break;
		}
	}

	if (oppositeUnit == nullptr) {
		std::cout << "Error in Unit::avoidOppositeUnit()" << std::endl;
		return;
	}

	//Check if the opposite unit intends to move to the tile that this unit currently stands on
	if (oppositeUnit->getPathP()->top()->getId() == _currentTileP->getId()) {
		//Units are blocking each other
		/*Loop through neighbours of this tile until I find one that is available. If I don't find any (extremely uunlikely),
		stop both units.
		*/
		Tile* availableTile = nullptr;
		std::vector<Tile*>* neighbours = _currentTileP->getNeighboursP();
		for (int i = 0; i < neighbours->size(); i++) {
			if ((*neighbours)[i]->isAvailable(_type)) {
				availableTile = (*neighbours)[i];
			}
		}
		if (availableTile == nullptr) {
			//Stop both units
			_wantsToMove = false;
			oppositeUnit->setWantsToMove(false);
		}
		//Add the current tile to the _path stack, then add the available neighbour tile
		_path.push(_currentTileP);
		_path.push(availableTile);
	}
}

void Unit::setPointersToThisUnit() {
	//This tile
	if (_type == Unit::Type::LAND) {
		_currentTileP->setLandUnitP(nullptr);
	}
	else {
		_currentTileP->setAirUnitP(nullptr);
	}

	//Next tile
	if (_type == Unit::Type::LAND) {
		_path.top()->setLandUnitP(this);
	}
	else {
		_path.top()->setAirUnitP(this);
	}
}

void Unit::move() {
	/* If the unit is moving, increment its internal clock (currentDistance) - how close it is to reaching the
	total distance. Once it reaches the total distance, it is now fully on the new tile and can move to another one.
	*/

	//TODO: Create x and y offset variables.
	//TODO: Change the x and y offset to make the movement animated.

	_currentDistance += _speed;

	//std::cout << "Current distance = " << _currentDistance << std::endl;

	if (_currentDistance >= _distance) {
		//Now the unit is on the new tile (technically it was there for a longer time but now it can move further)

		_currentDistance = 0;
		_moving = false;

	}
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

bool Unit::getHovered() {
	return _hovered;
}

bool Unit::getSelected() {
	return _selected;
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

void Unit::setHovered(bool hovered) {
	_hovered = hovered;
}

void Unit::setSelected(bool selected) {
	_selected = selected;
}