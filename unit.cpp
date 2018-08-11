#include "unit.h"
#include "tile.h"
#include "globals.h"

#include <iostream>

Unit::Unit() {}

Unit::Unit(Tile* currentTileP, Unit::Type type) :
	_currentTileP(currentTileP),
	_type(type)
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
		Tile::Occupancy occupancy = _path.top()->getOccupancy();
		if ((_type == Unit::Type::LAND && occupancy == Tile::Occupancy::LAND) ||
			(_type == Unit::Type::LAND && occupancy == Tile::Occupancy::LAND_AND_AIR) ||
			(_type == Unit::Type::AIR && occupancy == Tile::Occupancy::AIR) ||
			(_type == Unit::Type::AIR && occupancy == Tile::Occupancy::LAND_AND_AIR)) {

			//Desired tile is occupied, unit cannot move.
			//_moving = false;		//unnecessary
			std::cout << "Next tile is occupied!" << std::endl;
		}
		else {
			//Desired tile isn't occupied, unit can move.
			_moving = true;

			//Set distance
			_distance = _currentTileP->isNeighbourDiagonal(_path.top()) ?
				globals::TILE_DIAGONAL_DISTANCE : globals::TILE_STRAIGHT_DISTANCE;

			//Set the occupancy of the tile that this unit is leaving and the tile the unit is moving onto
			setThisAndNextOccupancies();

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

void Unit::setThisAndNextOccupancies() {
	//This tile
	Tile::Occupancy thisOccupancy = _currentTileP->getOccupancy();
	if (_type == Unit::Type::LAND) {
		if (thisOccupancy == Tile::Occupancy::LAND) {
			_currentTileP->setOccupancy(Tile::Occupancy::NONE);
		}
		else if (thisOccupancy == Tile::Occupancy::LAND_AND_AIR) {
			_currentTileP->setOccupancy(Tile::Occupancy::AIR);
		}
		else {
			//FIX THIS!
			std::cout << "Error when setting the occupancy 1 in unit.cpp" << std::endl;
		}
	}
	else {
		if (thisOccupancy == Tile::Occupancy::AIR) {
			_currentTileP->setOccupancy(Tile::Occupancy::NONE);
		}
		else if (thisOccupancy == Tile::Occupancy::LAND_AND_AIR) {
			_currentTileP->setOccupancy(Tile::Occupancy::LAND);
		}
		else {
			std::cout << "Error when setting the occupancy 2 in unit.cpp" << std::endl;
		}
	}

	//Next tile
	Tile::Occupancy nextOccupancy = _path.top()->getOccupancy();
	if (_type == Unit::Type::LAND) {
		if (nextOccupancy == Tile::Occupancy::NONE) {
			//std::cout << "Setting the next tile to be occupied by land." << std::endl;
			_path.top()->setOccupancy(Tile::Occupancy::LAND);
		}
		else if (nextOccupancy == Tile::Occupancy::AIR) {
			_path.top()->setOccupancy(Tile::Occupancy::LAND_AND_AIR);
		}
		else {
			std::cout << "Error when setting the occupancy 3 in unit.cpp" << std::endl;
		}
	}
	else {
		if (nextOccupancy == Tile::Occupancy::NONE) {
			_path.top()->setOccupancy(Tile::Occupancy::AIR);
		}
		else if (nextOccupancy == Tile::Occupancy::LAND) {
			_path.top()->setOccupancy(Tile::Occupancy::LAND_AND_AIR);
		}
		else {
			std::cout << "Error when setting the occupancy 4 in unit.cpp" << std::endl;
		}
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