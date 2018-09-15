#include "unit.h"
#include "tile.h"
#include "globals.h"
#include "pathfinder.h"
#include "pathparameters.h"
#include "map.h"

#include <iostream>

Unit::Unit() {}

Unit::Unit(Tile* currentTileP, Unit::Type type, Pathfinder* pathfinderP, Map* mapP) :
	_currentTileP(currentTileP),
	_type(type),
	_pathfinderP(pathfinderP),
	_mapP(mapP)
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

	if (_wantsToMove && !_moving) {
		Tile* nextTile = chooseNextTile();
		if (nextTile == nullptr) {
			//If the unit following the vector field reached the tagret, stop the unit.
			//std::cout << "nextTile = nullptr" << std::endl;
			_wantsToMove = false;
		}
		else {
			if (!canMoveToNextTile(nextTile)) {
				
				//Desired tile is occupied, unit cannot move.
				//std::cout << "Next tile is occupied!" << std::endl;

				/* If the unit is following a leader and got stuck on a stationary obstacle,
				now it's time to start following the vector field
				*/
				if (_followingLeader) {
					//This works because I only allow land units to be grouped
					Unit* nextTileUnit = nextTile->getLandUnitP();
					if (nextTileUnit == nullptr) {		//Wall
						_followingLeader = false;
					}
					else if (!nextTileUnit->getMoving() && nextTileUnit->getGroupId(false) != _groupId) {		//Stationary unit that is not from this group
						_followingLeader = false;
					}
				}
				else {
					std::cout << "This unit wants to move! " << rand() << std::endl;
					/* TODO
					Change this to avoiding units by taking a step 45 degrees from the planned path (next tile) if possible after
					waiting for the counter to reach the threshold. Only stop the unit if the next tile is still occupied and so 
					are the 2 tiles next to it.
					*/
					/* Stop the unit if it's following a vector field and encounters a stationary obstacle.
					This wouldn't be guaranteed to work if I just looked at nextTileUnit->getWantsToMove() because of the time
					the pathfinder takes to calculate the path. It would sometimes behave in weird ways.
					A better solution would is to create a counter that goes up by 1 each frame the unit is being blocked by 
					a stationary unit. When the counter reaches a certain limit (10 frames?) and the blocking unit is still there, 
					unwilling to move, this unit will finally decide to stop moving as well.
					*/
					Unit* nextTileUnit = nextTile->getLandUnitP();
					if (nextTileUnit == nullptr) {		//Test
						std::cout << "This shouldn't be possible" << std::endl;
					}

					if (!nextTileUnit->getWantsToMove()) {
						_shouldStopWantingToMoveCounter++;
						//std::cout << "Incrementing the counter! Now counter = " << _shouldStopWantingToMoveCounter << std::endl;
						/* I found out that when the units get stuck, they don't get to this point in here.

						I'm sure I found the reason, because it doesn't reach this block, but it reaches the block before this one. That means
						that !nextTileUnit->getWantsToMove() = false. In other words, the unit blocking this unit also wants to move.

						To fix this, I will need to update avoiding units.
						*/
						if (_shouldStopWantingToMoveCounter == _shouldStopWantingToMoveCounterThreshold) {
							//std::cout << "Setting _wantsToMove to false" << std::endl;
							_wantsToMove = false;
							_shouldStopWantingToMoveCounter = 0;
						}
					}
				}


				/* The unit on its way will probably encounter other units. This function handles how the unit reacts when
				one gets in the way.
				*/
				/* TODO
				Here, I need to add a way to avoid obstacles that weren't there at the creation of the vector field but are there now.
				*/
				//avoidDynamicObstacle();		//Currently commented out because it wouldn't work with group pathfinding
			}
			else {
				//Next tile isn't occupied, unit can start moving.
				_moving = true;

				//Reset the counter that determines if the unit should stop (if it's following a vector field)
				_shouldStopWantingToMoveCounter = 0;

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
				/* Sometimes, this can result in an id that's out of bounds of the array. In that case, the unit will stop
				following the leader and will return the group parent of the current tile.
				*/
				int tilesSize = _mapP->getColumns() * _mapP->getRows();
				if (newTileId > 0 && newTileId < tilesSize) {
					//Standard situation
					nextTile = _mapP->getTilesP()[newTileId];
				}
				else {
					//Out of bounds
					_followingLeader = false;
					nextTile = _currentTileP->getGroupParent(_groupId);
				}
			}
			else {
				nextTile = _currentTileP->getGroupParent(_groupId);		//This can be a nullptr
				/* nextTile can sometimes be a nullptr.
				This happens when a non-leader unit reaches the target destionation when following the vector field.
				This can also occasionally happen to the leader because of asynchonization.
				*/
			}
		}
	}
	catch(const char* msg) {
		std::cout << msg << std::endl;		//This should never happen
	}
	return nextTile;
}

bool Unit::canMoveToNextTile(Tile* nextTile) {
	if (nextTile->isAvailable(_type)) {
		int columnDiff = abs(_mapP->idToColumn(_currentTileP->getId()) - _mapP->idToColumn(nextTile->getId()));
		int rowDiff = abs(_mapP->idToRow(_currentTileP->getId()) - _mapP->idToRow(nextTile->getId()));
		if (columnDiff <= 1 && rowDiff <= 1) {
			return true;
		}
		return false;
	}
	return false;
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
	std::vector<Unit*>* _unitsP = _mapP->getUnitsP();
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
			if (_path.empty()) {
				_wantsToMove = false;
			}
		}
		else {
			if (_followingLeader && _leadersPathRelativeIdChange.empty()) {
				/* If this unit was following the leader and the leader stopped, it's possible that this unit will still be 
				far away from the leader. When this happens, the unit will switch to following the vector field until it
				finally reaches the leader or some other unit that are in the way.
				*/
				_followingLeader = false;
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