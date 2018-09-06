#include "unit.h"
#include "tile.h"
#include "globals.h"
#include "pathfinder.h"
#include "pathparameters.h"

#include <iostream>

Unit::Unit() {}

Unit::Unit(Tile* currentTileP, Tile** tiles, Unit::Type type, std::vector<Unit*>* unitsP, Pathfinder* pathfinderP) :
	_currentTileP(currentTileP),
	_tiles(tiles),
	_type(type),
	_unitsP(unitsP),
	_pathfinderP(pathfinderP)
{

}

Unit::~Unit() {

}

void Unit::update() {
	/* TODO
	I will need to update this once I implement group pathfinding. What if 2 units (both following a leader's relative path)
	go straight against each other and then block each other?
	*/

	/* If the unit wants to move and isn't moving yet, set moving to true (if it's possible) and
	calculate the distance it needs to travel (diagonal distance is longer).
	*/

	//Update variables (needed because the variables can be changed from another thread)
	updateVariables();

	/* DEBUGGING
	Just in case something changes from the other thread in the middle of the function, I will create copies of all the variables
	that can be changed from the other thread. Once the bug happens, I will look at the copies and compare them with the actual 
	variables.
	*/
	std::stack<Tile*> pathCopy = _path;
	std::stack<int> leadersPathRelativeIdChangeCopy = _leadersPathRelativeIdChange;
	bool followingLeaderCopy = _followingLeader;
	bool wantsToMoveCopy = _wantsToMove;
	int groupIdCopy = _groupId;

	/* TODO - fix a bug
	Sometimes, when I tell a group of units to go to a close tile and I keep clicking the tile, the formation of the group 
	unexpectedly changes. This can be easily replicated by creating a group of 4 units like this:
	............
	....O.......
	...O.O......
	....O.......
	............
	and then keep clicking on any of the close tiles. The formation won't remain in this position for too long.
	*/


	if (_wantsToMove && !_moving) {
		Tile* nextTile = chooseNextTile();
		if (nextTile == nullptr) {
			//In case nextTile is nullptr, it most likely means that the unit is following the vector field and reached the target tile.
			_wantsToMove = false;
		}
		else {
			//Check whether the next tile is occupied by a unit of the same type
			if (!nextTile->isAvailable(_type)) {

				//Desired tile is occupied, unit cannot move.
				//std::cout << "Next tile is occupied!" << std::endl;

				/* If the unit is following a leader and got stuck on a stationary obstacle,
				now it's time to start following the vector field
				*/
				//TODO: I completely forgot to include walls as stationary obstacles here
				if (_followingLeader && nextTile->getLandUnitP() != nullptr) {
					if (nextTile->getLandUnitP()->getMoving()) {			//Again, this works because I only allow land units to be grouped
						_followingLeader = false;
					}
				}


				/* The unit on its way will probably encounter other units. This function handles how the unit reacts when
				one gets in the way.
				*/
				//avoidDynamicObstacle();		//Currently commented out because it wouldn't work with group pathfinding
			}
			else {
				//Next tile isn't occupied, unit can start moving.
				_moving = true;

				//Set distance
				_distance = _currentTileP->isNeighbourDiagonal(nextTile) ?
					globals::TILE_DIAGONAL_DISTANCE : globals::TILE_STRAIGHT_DISTANCE;

				//Set pointers to this unit in the current tile and the next tile
				setPointersToThisUnit(nextTile);

				//Set the current tile to the tile the unit is moving onto
				_currentTileP = nextTile;

				//Remove the top element of the corresponding stack
				if (_groupId == -1) {
					_path.pop();
				}
				else {
					//Only pop the tile if the unit is following the leader
					if (_followingLeader) {
						if (_leadersPathRelativeIdChange.empty()) {
							std::cout << "This should never happen" << std::endl;
						}
						_leadersPathRelativeIdChange.pop();
					}
				}
			}
		}
	}

	//Move the unit
	if (_moving) {
		move();
	}
}

void Unit::updateVariables() {
	/* Variables _wantsToMove, _followingLeader, _path, _leadersPathRelativeIdChange and _groupId are shared between 2 threads.
	To prevent possible changes to some of the values while this function is running, I assign the values of the variables
	into temporary variables called "variable_name + New". At the start of this function, I assign the values from these
	temporary variables into the normal variables.
	*/
	if (_shouldUpdatePath) {
		_path = _pathNew;
		_shouldUpdatePath = false;
	}
	if (_shouldUpdateLeadersPathRelativeIdChange) {
		_leadersPathRelativeIdChange = _leadersPathRelativeIdChangeNew;
		_shouldUpdateLeadersPathRelativeIdChange = false;
	}
	if (_shouldUpdateFollowingLeaderNew) {
		_followingLeader = _followingLeaderNew;
		_shouldUpdateFollowingLeaderNew = false;
	}
	if (_shouldUpdateWantsToMoveNew) {
		_wantsToMove = _wantsToMoveNew;
		_shouldUpdateWantsToMoveNew = false;
	}
	if (_shouldUpdateGroupIdNew) {
		_groupId = _groupIdNew;
		_shouldUpdateGroupIdNew = false;
	}
}

Tile* Unit::chooseNextTile() {
	/* This function can reutrn a nullptr but that's fine because that only happens
	when the unit isn't following the leader and is on the target tile of the vector field.

	In very rare scenarios, it could also return a nullptr when the leader leads the unit
	to a tile that was previously occupied by an unmoving unit and therefore the tile isn't
	in the vector field. Given how rare this is, I think it's fine to just leave it and
	just accept the fact that the unit will stop on that tile in that situation.
	TODO: Test this situation to make sure this exact behaviour happens.
	*/
	Tile* nextTile = nullptr;
	try {
		if (_groupId == -1) {
			if (_path.empty()) {
				throw "Error in unit.cpp! _path is empty.";		//This should never happen
			}
			nextTile = _path.top();
		}
		else {
			if (_followingLeader) {
				if (_leadersPathRelativeIdChange.empty()) {
					throw "Error in unit.cpp! _leadersPathRelativeIdChange is empty.";		//This should never happen
				}
				int newTileId = _currentTileP->getId() + _leadersPathRelativeIdChange.top();
				nextTile = _tiles[newTileId];
			}
			else {
				nextTile = _currentTileP->getGroupParent(_groupId);		//This can be a nullptr
			}
		}
	}
	catch(const char* msg) {
		std::cout << msg << std::endl;		//This should never happen
	}
	return nextTile;
}

void Unit::avoidDynamicObstacle() {
	/* The unit on its way will probably encounter other units. This function handles how the unit reacts when
	one gets in the way.
	*/
	/* When 2 units are moving towards each other from opposite directions, they would get stuck.
	This makes one unit get out of the way, let the other unit pass and then move back and continue.
	*/

	/* TODO
	This function needs to get updated to work with group pathfinding as well.
	*/

	//Select the unit that's in the way
	Unit* blockingUnit = nullptr;
	for (int i = 0; i < _unitsP->size(); i++) {
		if ((*_unitsP)[i]->getCurrentTileP()->getId() == _path.top()->getId()) {
			blockingUnit = (*_unitsP)[i];
			break;
		}
	}

	if (blockingUnit == nullptr) {
		std::cout << "Error in Unit::avoidDynamicObstacle()" << std::endl;
		return;
	}
	
	//Check if the blocking unit hasn't finished its path yet
	if (!blockingUnit->getPathP()->empty()) {
		/* Check if the blocking unit intends to move to the tile that this unit currently stands on. If both units waited
		on each other to make a move, it would result in a deadlock. I need to handle the situation, therefore one unit
		will get out of the way, let the other one pass and then go back and continue in the original path.

		If the blocking unit intends to move to another tile (not this one), then I won't do anything and I will just wait.
		*/
		if (blockingUnit->getPathP()->top()->getId() == _currentTileP->getId()) {
			//Units are blocking each other

			/* Loop through neighbours of this tile until I find one that is available. If I don't find any (extremely unlikely),
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
				std::cout << "This block of code should happen extremely rarely" << std::endl;
				//Stop both units
				_wantsToMove = false;
				blockingUnit->setWantsToMove(false, false);		
				//I actually haven't tested this block of code yet because it's hard to simulate this situation.
			}
			//Add the current tile to the _path stack, then add the available neighbour tile
			_path.push(_currentTileP);
			_path.push(availableTile);
		}
	}
	else {
		//If the unit that's blocking this unit stopped moving, I need to find a new path.
		_wantsToMove = false;		//Stop this unit, in case a path cannot be found

		//Check if the target tile is available
		while (_path.size() != 1) {
			_path.pop();
		}
		Tile* targetTileP = _path.top();
		if (targetTileP->isAvailableForPathfinding(_type)) {
			//Set the path parameters
			
			std::vector<Unit*> unitGroup;		//group of 1 unit
			unitGroup.push_back(this);
			PathParameters* parameters = new PathParameters(targetTileP, unitGroup, -1);		//Deletion is handled in Pathfinder
			_pathfinderP->pushPathParameters(parameters);

			//Notify the other thread
			_pathfinderP->getCondP()->notify_one();
			
		}
		

		/* POSSIBLE OPTIMIZATION
		Right now, if I get blocked by a unit, I recalculate the whole path from the current point to the intended target.
		Instead, I could only calculate the path to the nearest available tile on the original path, then add the rest out the
		original path to the new path. That way, I would only calculate just enough to get past the obstacle and wouldn't have to
		calculate what has been calculated before. But this could be pretty tricky to implement.
		*/
	}
}

void Unit::setPointersToThisUnit(Tile* nextTile) {
	//This tile
	if (_type == Unit::Type::LAND) {
		_currentTileP->setLandUnitP(nullptr);
	}
	else {
		_currentTileP->setAirUnitP(nullptr);
	}

	//Next tile
	if (_type == Unit::Type::LAND) {
		nextTile->setLandUnitP(this);
	}
	else {
		nextTile->setAirUnitP(this);
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
		//Now the unit is fully on the new tile (technically it was there for a longer time but now it can move further)
		_currentDistance = 0;
		_moving = false;

		//Stop the unit if it reached its destination
		if (_groupId == -1) {
			if (_path.size() == 0) {
				_wantsToMove = false;
				_moving = false;
			}
		}
		else {
			if (_leadersPathRelativeIdChange.size() == 0) {
				_wantsToMove = false;
				_moving = false;
			}
		}
		
	}
}

int Unit::getGroupId(bool isNew) {
	if (isNew) {
		return _groupIdNew;
	}
	else {
		return _groupId;
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

void Unit::setGroupId(int groupId, bool isFromOtherThread) {
	if (isFromOtherThread) {
		_groupIdNew = groupId;
		_shouldUpdateGroupIdNew = true;
	}
	else {
		_groupId = groupId;
	}
}

void Unit::setWantsToMove(bool wantsToMove, bool isFromOtherThread) {
	if (isFromOtherThread) {
		_wantsToMoveNew = wantsToMove;
		_shouldUpdateWantsToMoveNew = true;
	}
	else {
		_wantsToMove = wantsToMove;
	}
}

void Unit::setMoving(bool moving) {
	_moving = moving;
}

void Unit::setPath(std::stack<Tile*> path, bool isFromOtherThread) {
	if (isFromOtherThread) {
		_pathNew = path;
		_shouldUpdatePath = true;
	}
	else {
		_path = path;
	}
}

void Unit::setLeadersPathRelativeIdChange(std::stack<int> path, bool isFromOtherThread) {
	if (isFromOtherThread) {
		_leadersPathRelativeIdChangeNew = path;
		_shouldUpdateLeadersPathRelativeIdChange = true;
	}
	else {
		_leadersPathRelativeIdChange = path;
	}
}

void Unit::setFollowingLeader(bool followingLeader, bool isFromOtherThread) {
	if (isFromOtherThread) {
		_followingLeaderNew = followingLeader;
		_shouldUpdateFollowingLeaderNew = true;
	}
	else {
		_followingLeader = followingLeader;
	}
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