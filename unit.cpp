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

	/* TODO
	What I need to implement:
	An efficient unit avoidance if 2 units are blocking each other. I will probably need to check if the unit 
	is following the leader, because if one is and the other one isn't, it should probably be the one that isn't following that moves back
	and creates space. If both are following their leaders (in order for this to happen, they must be from 2 different groups), then I will
	set one unit to start following the vector field, and then I can work with it like in the previous scenario - the vector field unit
	will create space. If both units are folowing the vector field, then it doesn't matter which unit creates space, so it will be the one
	that reaches the block of code first. As for the avoidance itself, the units should try to get as close as possible to the occupied tile.
	In the old version, I chose the first available tile, which was often very inefficient. Instead, the unti should move to a tile with a 
	45 degree difference, if possible, and if it's not possible, continue increasing the amount by 45 degrees.
	*/

	/* TODO
	Once the unit avoidance is done, I will implement unit rotation and fluent movement (animation).
	*/

	//Update variables (needed because the variables can be changed from another thread)
	updateVariables();

	if (_wantsToMove && !_moving) {
		Tile* nextTile = chooseNextTile();
		if (nextTile == nullptr) {
			//If the unit following the vector field reached the tagret, stop the unit.
			_wantsToMove = false;
		}
		else {
			if (!canMoveToNextTile(nextTile)) {		
				//Desired tile is occupied, unit cannot move.
				/* If the unit is following a leader and got stuck on a stationary obstacle,
				now it's time to start following the vector field
				*/
				if (_followingLeader) {
					//This works because I only allow land units to be grouped
					Unit* nextTileUnit = nextTile->getLandUnitP();
					if (nextTileUnit == nullptr) {		//Wall
						_followingLeader = false;
					}
					else if (!nextTileUnit->getWantsToMove() && nextTileUnit->getGroupId(false) != _groupId) {		//Stationary unit that is not from this group
						_followingLeader = false;
					}
					else if (!nextTileUnit->getWantsToMove()) {
						/* Sometimes, when the units follow the leader, they might get stuck and they wouldn't form the cluster after 
						arriving at the target location, and remain in a long "tail" instead. This here fixes that problem: if this unit
						is being blocked by a stationary unit from this group, increment a counter. When the counter reaches a certain 
						threshold, stop following the leader.
						The counter needs to be here because this block of code can happen in normal situations, e. g. when a big, 
						clustered group of units is ordered to move. When the unit successfuly moves, the counter resets,
						therefore only the units that are really stuck stop following the leader.
						*/
						_shouldStopFollowingLeaderCounter++;
						if (_shouldStopFollowingLeaderCounter == _shouldStopFollowingLeaderThreshold) {
							_followingLeader = false;		
							_shouldStopFollowingLeaderCounter = 0;
						}
					}
				}
				else {
					/* If the unit is following the vector field and encounters an obstacle, increment a counter. If this happens
					multiple frames in a row, it probably means that the unit is stationary. When the counter reaches a threshold,
					chooseNextTile() will return a tile next to the original tile (the one being occupied by another unit). This way,
					the unit will avoid stationary obstacles.
					*/
					Unit* nextTileUnit = nextTile->getLandUnitP();
					if (nextTileUnit == nullptr) {		//Test
						std::cout << "This shouldn't be possible" << std::endl;
					}

					if (!nextTileUnit->getWantsToMove()) {
						_shouldTryToAvoidStationaryObstacleCounter++;
					}
				}


				/* The unit on its way will probably encounter other units. This function handles how the unit reacts when
				one gets in the way.
				*/
				/* TODO
				Here, I need to add a way to avoid obstacles that weren't there at the creation of the vector field but are there now.
				*/
				avoidDynamicObstacle(nextTile);		//Currently commented out because it wouldn't work with group pathfinding
			}
			else {
				//Next tile isn't occupied, unit can start moving.
				_moving = true;

				/* Reset the counter that determines if the unit should stop following the leader (if it's following the leader).
				Reset the counter that determines if the unit should try to avoid an obstacle (if it's following a vector field).
				*/
				_shouldStopFollowingLeaderCounter = 0;
				_shouldTryToAvoidStationaryObstacleCounter = 0;

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
	/* This function can return a nullptr but that's fine because that only happens
	when the unit isn't following the leader and is on the target tile of the vector field.

	In very rare scenarios, it could also return a nullptr when the leader leads the unit
	to a tile that was previously occupied by an unmoving unit and therefore the tile isn't
	in the vector field. Given how rare this is, I think it's fine to just leave it and
	just accept the fact that the unit will stop on that tile in that situation.
	
	The same can happen when the unit is following the vector field and avoids a stationary 
	unit by going 45 degrees from the next planned tile (this tile can be can be absent in 
	the vector field)
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
				nextTile = getNextTileIfFollowingLeader();
			}
			else {
				nextTile = getNextTileIfFollowingVectorField();
			}
		}
	}
	catch(const char* msg) {
		std::cout << msg << std::endl;		//This should never happen
	}
	return nextTile;
}

Tile* Unit::getNextTileIfFollowingLeader() {
	Tile* nextTile = nullptr;
	if (_leadersPathRelativeIdChange.empty()) {
		throw "Error in unit.cpp! _leadersPathRelativeIdChange is empty.";		//This should never happen
	}
	int newTileId = _currentTileP->getId() + _leadersPathRelativeIdChange.top();
	/* Sometimes, this can result in an id that's out of bounds of the array. In that case, the unit will stop
	following the leader and will return the group parent of the current tile.
	*/
	if (!wouldTileBeOutOfBounds(newTileId)) {
		//Standard situation
		nextTile = _mapP->getTilesP()[newTileId];
	}
	else {
		//Out of bounds
		_followingLeader = false;
		nextTile = getNextTileIfFollowingVectorField();
	}

	//Stop following the leader if it would mean going in the wrong direction.
	if (_followingLeader && wouldFollowingLeaderResultInWrongDirection(nextTile)) {
		_followingLeader = false;
		nextTile = getNextTileIfFollowingVectorField();
	}

	return nextTile;
}

Tile* Unit::getNextTileIfFollowingVectorField() {
	Tile* nextTile = nullptr;
	if (_shouldTryToAvoidStationaryObstacleCounter != _shouldTryToAvoidStationaryObstacleThreshold) {
		//Normal situation
		nextTile = _currentTileP->getGroupParent(_groupId);		//This can be a nullptr
		/* nextTile can sometimes be a nullptr.
		This happens when a non-leader unit reaches the target destionation when following the vector field.
		This can also occasionally happen to the leader because of asynchonization.
		*/
	}
	else {
		/* The tile that is pointed to by current tile's parent is blocked by a non-moving unit. Check the 2 closest tiles and if
		at least one of them is available, assign nextTile to that tile. Otherwise, stop the unit by assigning nullptr to nextTile.
		(the update function assigns false to _wantsToMove if nextTile is nullptr)

		If the parent of the current tile is the target tile, stop moving. Otherwise, it would create an infinite loop of jumping
		around the target tile.
		*/
		if (_currentTileP->getGroupParent(_groupId)->getGroupParent(_groupId) == nullptr) {
			//std::cout << "Reached the target tile." << std::endl;
			_wantsToMove = false;
			_shouldTryToAvoidStationaryObstacleCounter = 0;
		}
		else {
			nextTile = tryToFindCloseAvailableTile();
			_shouldTryToAvoidStationaryObstacleCounter = 0;
		}

	}

	return nextTile;
}

bool Unit::wouldTileBeOutOfBounds(int tileId) {
	int tilesSize = _mapP->getColumns() * _mapP->getRows();
	return !(tileId > 0 && tileId < tilesSize);
}

bool Unit::wouldFollowingLeaderResultInWrongDirection(Tile* untestedNextTile) {
	/* Wrong direction means having a difference of 135 or more degrees:
	If following the leader's path means going in the wrong direction, units won't follow the leader.
	Returns true if the difference between leader's path and vector path is 135 or more degrees.
	*/

	
	//Some maths stuff. This should always work unless the map is extremely small.
	int leadersRowChange = _leadersPathRelativeIdChange.top() / (_mapP->getColumns() - 1);
	int leadersColumnChange = _leadersPathRelativeIdChange.top() - leadersRowChange * _mapP->getColumns();

	int vectorRowChange = _mapP->idToRow(_currentTileP->getGroupParent(_groupId)->getId()) - _mapP->idToRow(_currentTileP->getId());
	int vectorColumnChange = _mapP->idToColumn(_currentTileP->getGroupParent(_groupId)->getId()) - _mapP->idToColumn(_currentTileP->getId());

	if (leadersRowChange == 0 && leadersColumnChange == 0) {
		std::cout << "This should never happen" << std::endl;
	}
	if (leadersRowChange == 1 && leadersColumnChange == 1) {
		//Leader moved diagonally
		if (abs(leadersRowChange - vectorRowChange) + abs(leadersColumnChange - vectorColumnChange) <= 2) {
			return false;
		}
	}
	else {
		//Leader moved straight
		if (abs(leadersRowChange - vectorRowChange) < 2 && abs(leadersColumnChange - vectorColumnChange) < 2) {
			return false;
		}
	}
	return true;
}

Tile* Unit::tryToFindCloseAvailableTile() {
	/* The tile that is pointed to by current tile's parent is blocked by a non-moving unit. Check the 2 closest tiles and if
	at least one of them is available, assign nextTile to that tile. Otherwise, stop the unit by assigning nullptr to nextTile.
	(the update function assigns false to _wantsToMove if nextTile is nullptr)
	*/

	/* I need a way to find out which tiles are close. Close tile is the one that has a 45 degree angle between the parent vector
	and the vector to this tile. A few examples:
	....C..........UC.........COC......................
	...UO..........CO..........U...........CU..........
	....C..................................OC..........
	U = unit, O = obstacle, C = close tile
	*/

	int currentRow = _mapP->idToRow(_currentTileP->getId());
	int currentColumn = _mapP->idToColumn(_currentTileP->getId());
	int rowChange = _mapP->idToRow(_currentTileP->getGroupParent(_groupId)->getId()) - currentRow;
	int columnChange = _mapP->idToColumn(_currentTileP->getGroupParent(_groupId)->getId()) - currentColumn;

	int tile1id;
	int tile2id;

	if (rowChange == 0 && columnChange == 0) {
		std::cout << "This should never happen" << std::endl;
	}
	if (rowChange == 0 || columnChange == 0) {
		//Original next tile is straight from the current tile
		//To get the close tiles in this case, I need to add or subtract 1 from the variable that equals 0
		if (rowChange == 0) {
			tile1id = _mapP->positionToId(currentRow + 1, currentColumn + columnChange);
			tile2id = _mapP->positionToId(currentRow - 1, currentColumn + columnChange);
		}
		else {
			tile1id = _mapP->positionToId(currentRow + rowChange, currentColumn + 1);
			tile2id = _mapP->positionToId(currentRow + rowChange, currentColumn - 1);
		}
		
	}
	else {
		//Original next tile is diagonal from the current tile
		//To get the close tiles in this case, I need to set one of the differences to 0
		tile1id = _mapP->positionToId(currentRow, currentColumn + columnChange);
		tile2id = _mapP->positionToId(currentRow + rowChange, currentColumn);
	}

	//std::cout << "Trying to find close available tile" << std::endl;

	//Check if the first close tile fits the requirements
	if (!wouldCloseTileCrossBorder(tile1id)) {
		Tile* closeTile1 = _mapP->getTilesP()[tile1id];
		//This condition works because I only allow group pathfinding for land units.
		if (closeTile1->isAvailableForPathfinding(Unit::Type::LAND)) {	
			//std::cout << "Close tile 1" << std::endl;
			return closeTile1;
		}
	}
	
	//closeTile1 would either cross the border or the tile isn't available (it is occupied). I need to check the other tile now.
	if (!wouldCloseTileCrossBorder(tile2id)) {
		Tile* closeTile2 = _mapP->getTilesP()[tile2id];
		//This condition works because I only allow group pathfinding for land units.
		if (closeTile2->isAvailableForPathfinding(Unit::Type::LAND)) {
			//std::cout << "Close tile 2" << std::endl;
			return closeTile2;
		}
	}

	//Neither one of the close tiles fits the requirements. Return nullptr.
	//std::cout << "No close tile." << std::endl;
	return nullptr;		
}

bool Unit::wouldCloseTileCrossBorder(int tileId) {
	//Check if it leaves array bounds (= crossing on the top or bottom side)
	if (!wouldTileBeOutOfBounds(tileId)) {
		//Check if it crosses on the left or right side
		int columnDiff = abs(_mapP->idToColumn(_currentTileP->getId()) - _mapP->idToColumn(tileId));
		if (columnDiff <= 1) {
			return false;
		}
	}
	return true;
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

void Unit::avoidDynamicObstacle(Tile* nextTile) {
	/* The unit on its way will probably encounter other units. This function handles how the unit reacts when
	one gets in the way.
	*/
	/* When 2 units are moving towards each other from opposite directions, they would get stuck.
	This makes one unit get out of the way, let the other unit pass and then move back and continue.
	*/

	/* TODO
	Completely rework this function.

	What this function needs to do:
	1) If the unit is not in a group:
		a) When it encounters a unit that is not moving, find another path to the target.
	*/

	//Select the unit that's in the way
	Unit* blockingUnit = nullptr;
	if (_type == Unit::Type::LAND) {
		blockingUnit = nextTile->getLandUnitP();
	}
	else {
		blockingUnit = nextTile->getAirUnitP();
	}

	if (blockingUnit == nullptr) {
		std::cout << "Error in Unit::avoidDynamicObstacle()" << std::endl;
		return;
	}

	//Check if the blocking stopped and doesn't want to move anymore
	if (blockingUnit->getWantsToMove()) {
		/* Blocking unit hasn't finished its path yet, wants to move.

		First, this unit will wait. The blocking unit is either going somewhere a will soon go away and this
		unit can continue in its way, or wants to go to this tile. If it wants to go to this tile, it will 
		stop moving soon but will still want to move.
		*/
		if (!blockingUnit->getMoving()) {
			//The blocking unit stopped because of an obstacle, but it still doesn't have to be this unit that's blocking it
			/* I should be able to use the chooseNextTile() function because _wantsToMove is true, _moving is false and
			a situation where it returns nullptr shouldn't be possible.

			If I'm wrong, this will need a lot more work.
			*/
			Tile* blockingUnitNextTile = blockingUnit->chooseNextTile();		
			if (blockingUnitNextTile == nullptr) {
				std::cout << "Something went wrong" << std::endl;		//Shit. This code was reached when I made a group of units move.
			}
			int blockingUnitNextTileId = blockingUnitNextTile->getId();
			if (blockingUnitNextTileId == _currentTileP->getId()) {
				std::cout << "Units are blocking each other" << std::endl;
			}
		}
	}
	else {
		/* Blockung unit has stopped, doesn't want to move.

		If this unit is in a group, stationary unit avoidance is already taken care of in the chooseNextTile() function,
		therefore here I only need to work with units which aren't in a group.
		*/
		if (_groupId == -1) {
			//Find a new path to the target
			_wantsToMove = false;		//Stop this unit, in case a path cannot be found or the target isn't available anymore

			//Select the target tile
			while (_path.size() != 1) {
				_path.pop();
			}
			Tile* targetTileP = _path.top();

			//Check if the target tile is available
			/* TODO - IMPORTANT
			Instead if checking whether the target tile is available and completely stopping the unit if it isn't, I should always
			find a path. If the target tile isn't available, then the new target will be the closest tile to it.
			*/
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

			I will only do this if I need to get abolutely perfect performace, as this would really be hard to implement.
			*/
		}
	}
	
	//WHY THE FEHUE WAS I DOING IT LIKE THIS?! IT MAKES NO SENSE AT ALL
	/*
	//Check if the blocking unit hasn't finished its path yet
	if (!blockingUnit->getPathP()->empty()) {
		// Check if the blocking unit intends to move to the tile that this unit currently stands on. If both units waited
		//on each other to make a move, it would result in a deadlock. I need to handle the situation, therefore one unit
		//will get out of the way, let the other one pass and then go back and continue in the original path.

		//If the blocking unit intends to move to another tile (not this one), then I won't do anything and I will just wait.
		
		if (blockingUnit->getPathP()->top()->getId() == _currentTileP->getId()) {
			//Units are blocking each other

			// Loop through neighbours of this tile until I find one that is available. If I don't find any (extremely unlikely),
			//stop both units.
			//
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
		

		// POSSIBLE OPTIMIZATION
		//Right now, if I get blocked by a unit, I recalculate the whole path from the current point to the intended target.
		//Instead, I could only calculate the path to the nearest available tile on the original path, then add the rest out the
		//original path to the new path. That way, I would only calculate just enough to get past the obstacle and wouldn't have to
		//calculate what has been calculated before. But this could be pretty tricky to implement.
		//
	}
	*/
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
	//Use isNew = false for standard calls of this function
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