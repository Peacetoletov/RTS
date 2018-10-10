#include "engine.h"
#include "unit.h"
#include "pathfinder.h"
#include "map.h"
#include "tile.h"
#include "globals.h"
#include "pathparameters.h"

#include <iostream>

Engine::Engine() {}

Engine::Engine(Unit* unit, Pathfinder* pathfinderP, Map* mapP) :
	_unit(unit),
	_pathfinderP(pathfinderP),
	_mapP(mapP)
{

}

void Engine::update() {
	/* If the unit wants to move and isn't moving yet, set moving to true (if it's possible) and
	calculate the distance it needs to travel (diagonal distance is longer).
	*/

	/* TODO
	Once the unit avoidance is done, I will refactor a lot of this code. I will create a class called Gps, Wheel, Engine or something like
	that and put everything that has something to do with movement and pathfinding in there.
	*/

	/* TODO
	Once refactoring, I will implement unit rotation and fluent movement (animation).
	*/

	/* TODO
	Test what happens when I create more than 100 vector fields while some units are following one.
	*/

	//Update variables (needed because the variables can be changed from another thread)
	updateVariables();

	if (_wantsToMove && !_moving) {
		//Tile* nextTile = chooseNextTile();
		_intendedNextTile = chooseNextTile();
		if (_intendedNextTile == nullptr) {
			//If the unit following the vector field reached the tagret, stop the unit.
			_wantsToMove = false;
		}
		else {
			if (!canMoveToNextTile(_intendedNextTile)) {
				//Desired tile is occupied, unit cannot move.
				/* If the unit is following a leader and got stuck on a stationary obstacle,
				now it's time to start following the vector field
				*/
				if (_followingLeader) {
					//This works because I only allow land units to be grouped
					Unit* nextTileUnit = _intendedNextTile->getLandUnitP();		
					if (nextTileUnit == nullptr) {		//Wall
						_followingLeader = false;
					}
					else if (!nextTileUnit->getEngineP()->getWantsToMove() && nextTileUnit->getEngineP()->getGroupId(false) != _groupId) {		//Stationary unit that is not from this group
						_followingLeader = false;
					}
					else if (!nextTileUnit->getEngineP()->getWantsToMove()) {
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
					Unit* nextTileUnit = _intendedNextTile->getLandUnitP();
					if (nextTileUnit->getEngineP() == nullptr) {		//Test
						std::cout << "This shouldn't be possible" << std::endl;
					}

					if (!nextTileUnit->getEngineP()->getWantsToMove()) {
						_shouldTryToAvoidStationaryObstacleCounter++;
					}
				}


				/* The unit on its way will probably encounter other units. This function handles how the unit reacts when
				one gets in the way.
				*/
				/* TODO
				Here, I need to add a way to avoid obstacles that weren't there at the creation of the vector field but are there now.
				*/
				if (_intendedNextTile->canUnitMoveOnThisTerrain(_unit->getType())) {		//This prevents calling the function if this unit intends to go into a wall
					//This function can change _intendedNextTile
					avoidDynamicObstacle();
				}
			}
			if (canMoveToNextTile(_intendedNextTile)) {		//I'm using this condition instead of a simple else because _intendedNextTile can change
				//Next tile isn't occupied, unit can start moving.
				_moving = true;

				/* Reset the counter that determines if the unit should stop following the leader (if it's following the leader).
				Reset the counter that determines if the unit should try to avoid an obstacle (if it's following a vector field).
				*/
				_shouldStopFollowingLeaderCounter = 0;
				_shouldTryToAvoidStationaryObstacleCounter = 0;

				//Set distance
				_distance = _unit->getCurrentTileP()->isNeighbourDiagonal(_intendedNextTile) ?
					globals::TILE_DIAGONAL_DISTANCE : globals::TILE_STRAIGHT_DISTANCE;

				//Set pointers to this unit in the current tile and the next tile
				setPointersToThisUnit(_intendedNextTile);

				//Set the current tile to the tile the unit is moving onto
				_unit->setCurrentTileP(_intendedNextTile);

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

void Engine::updateVariables() {
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

Tile* Engine::chooseNextTile() {
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
	catch (const char* msg) {
		std::cout << msg << std::endl;		//This should never happen
	}
	return nextTile;
}

Tile* Engine::getNextTileIfFollowingLeader() {
	Tile* nextTile = nullptr;
	if (_leadersPathRelativeIdChange.empty()) {
		throw "Error in unit.cpp! _leadersPathRelativeIdChange is empty.";		//This should never happen
	}
	int newTileId = _unit->getCurrentTileP()->getId() + _leadersPathRelativeIdChange.top();
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

Tile* Engine::getNextTileIfFollowingVectorField() {
	Tile* nextTile = nullptr;
	if (_higherPriorityTileInVectorField != nullptr) {
		nextTile = _higherPriorityTileInVectorField;
		_higherPriorityTileInVectorField = nullptr;
	}
	else if (_shouldTryToAvoidStationaryObstacleCounter != _shouldTryToAvoidStationaryObstacleThreshold) {
		//Normal situation
		nextTile = _unit->getCurrentTileP()->getGroupParent(_groupId);		//This can be a nullptr
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
		if (_unit->getCurrentTileP()->getGroupParent(_groupId)->getGroupParent(_groupId) == nullptr) {
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

bool Engine::wouldTileBeOutOfBounds(int tileId) {
	int tilesSize = _mapP->getColumns() * _mapP->getRows();
	return !(tileId > 0 && tileId < tilesSize);
}

bool Engine::wouldFollowingLeaderResultInWrongDirection(Tile* untestedNextTile) {
	/* Wrong direction means having a difference of 135 or more degrees:
	If following the leader's path means going in the wrong direction, units won't follow the leader.
	Returns true if the difference between leader's path and vector path is 135 or more degrees.
	*/


	//Some maths stuff. This should always work unless the map is extremely small.
	int leadersRowChange = _leadersPathRelativeIdChange.top() / (_mapP->getColumns() - 1);
	int leadersColumnChange = _leadersPathRelativeIdChange.top() - leadersRowChange * _mapP->getColumns();

	int vectorRowChange = _mapP->idToRow(_unit->getCurrentTileP()->getGroupParent(_groupId)->getId())
		- _mapP->idToRow(_unit->getCurrentTileP()->getId());			//THIS RESULTED IN AN ERROR
	/* Alright. How the fuck did this happen. I was testing it for 5 minutes or so and already got this error 3 times. The problem is
	with the second part: " _mapP->idToRow(_unit->getCurrentTileP()->getId()) ". I have no idea how _unit->getCurrentTileP() can
	return nullptr. That's literally impossible.

	TODO: Fix this!
	*/

	int vectorColumnChange = _mapP->idToColumn(_unit->getCurrentTileP()->getGroupParent(_groupId)->getId()) - _mapP->idToColumn(_unit->getCurrentTileP()->getId());

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

Tile* Engine::tryToFindCloseAvailableTile() {
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

	int currentRow = _mapP->idToRow(_unit->getCurrentTileP()->getId());
	int currentColumn = _mapP->idToColumn(_unit->getCurrentTileP()->getId());
	int rowChange = _mapP->idToRow(_unit->getCurrentTileP()->getGroupParent(_groupId)->getId()) - currentRow;
	int columnChange = _mapP->idToColumn(_unit->getCurrentTileP()->getGroupParent(_groupId)->getId()) - currentColumn;

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

bool Engine::wouldCloseTileCrossBorder(int tileId) {
	//Check if it leaves array bounds (= crossing on the top or bottom side)
	if (!wouldTileBeOutOfBounds(tileId)) {
		//Check if it crosses on the left or right side
		int columnDiff = abs(_mapP->idToColumn(_unit->getCurrentTileP()->getId()) - _mapP->idToColumn(tileId));
		if (columnDiff <= 1) {
			return false;
		}
	}
	return true;
}

bool Engine::canMoveToNextTile(Tile* nextTile) {
	if (nextTile->isAvailable(_unit->getType())) {
		int columnDiff = abs(_mapP->idToColumn(_unit->getCurrentTileP()->getId()) - _mapP->idToColumn(nextTile->getId()));
		int rowDiff = abs(_mapP->idToRow(_unit->getCurrentTileP()->getId()) - _mapP->idToRow(nextTile->getId()));
		if (columnDiff <= 1 && rowDiff <= 1) {
			return true;
		}
		return false;
	}
	return false;
}

void Engine::avoidDynamicObstacle() {
	/* The unit on its way will probably encounter other units. This function handles how the unit reacts when
	one gets in the way.
	When 2 units are moving towards each other from opposite directions, they would get stuck.
	This makes one unit get out of the way, let the other unit pass and then move back and continue.
	*/

	//Select the unit that's in the way
	Engine* blockingUnitEngine = nullptr;
	if (_unit->getType() == Unit::Type::LAND) {
		blockingUnitEngine = _intendedNextTile->getLandUnitP()->getEngineP();
	}
	else {
		blockingUnitEngine = _intendedNextTile->getAirUnitP()->getEngineP();
	}

	if (blockingUnitEngine == nullptr) {
		std::cout << "Error in Unit::avoidDynamicObstacle()" << std::endl;
		return;
	}

	//Check if the blocking stopped and doesn't want to move anymore
	if (blockingUnitEngine->getWantsToMove()) {
		/* Blocking unit hasn't finished its path yet, wants to move.

		Check if the blocking unit intends to go to this tile
		*/
		if (blockingUnitEngine->getIntentedNextTile()->getId() == _unit->getCurrentTileP()->getId()) {
			/* Units are blocking each other

			Rule list (which one of the 2 units will be going to the side to make room for the other one):
			1. If one unit is not in a group and the other one is, the one in the group will make room. It doesn't matter
			whether the group unit is following the leader or the vector field.
			2. If neither unit is in a group, the second unit to reach this code will make room.
			3. If both units are in a group, the second unit to reach this code will make room.

			Priority variable: when the rule states that the second unit will make room,
			(therefore the first unit has a higher priority), the first unit will switch
			its priority variable.
			*/

			if (_groupId == -1 && blockingUnitEngine->getGroupId(false) != -1) {
				/* The other unit will make room. This is so that I don't have to find a new path.
				No code here.		//I know this code is pointless but I find it more readable

				Nevermind I actually need code here.
				*/


				//This unit has the priority; the other unit will make room
				_hasHigherPriority = true;					//set the priority variable
			}
			else {
				/* Go to any neighbour tile, wait for the other unit to pass, then get back to this tile
				and continue in the original direction.

				This is done this way so that I don't need to calculate a new path.
				*/

				Tile* newTile = getAnyAvailableNeighbourTile();
				if (newTile != nullptr) {
					if (blockingUnitEngine->getHasHigherPriority()) {
						//The other unit has the priority; this unit will make room
						blockingUnitEngine->setHasHigherPriority(false);		//reset the priority variable

						//Push the additional tiles to the corresponding stack
						if (_groupId == -1) {
							_path.push(_unit->getCurrentTileP());
							_path.push(newTile);
						}
						else {
							if (_followingLeader) {
								int idDifference = newTile->getId() - _unit->getCurrentTileP()->getId();
								_leadersPathRelativeIdChange.push(-idDifference);
								_leadersPathRelativeIdChange.push(idDifference);
							}
							else {
								_higherPriorityTileInVectorField = newTile;
							}
						}

					}
					else {
						//This unit has the priority; the other unit will make room
						_hasHigherPriority = true;					//set the priority variable
					}
				}
				else {
					//If there is no available neighbour tile, stop the unit.
					_wantsToMove = false;
				}
			}
		}

	}
	else {
		/* Blocking unit has stopped, doesn't want to move.

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
			if (targetTileP->isAvailableForPathfinding(_unit->getType())) {
				//Set the path parameters
				std::vector<Unit*> unitGroup;		//group of 1 unit
				unitGroup.push_back(_unit);
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
}

Tile* Engine::getAnyAvailableNeighbourTile() {
	/* TODO: This could be improved. In the current situation, if 2 units block each other
	vertically, there is a chance that the unit making room will be forced to make room
	for a much longer time (many tiles) than is needed (1 tile).
	*/

	/* Go through all neighbour tiles. If any of them is available, return it.
	Start with straight neighbours, as the distance is shorter.
	*/
	int thisTileId = _unit->getCurrentTileP()->getId();
	int neighbourId;

	//Up
	neighbourId = thisTileId - _mapP->getColumns();
	if (!wouldCloseTileCrossBorder(neighbourId)) {
		Tile* neighbour = _mapP->getTilesP()[neighbourId];
		if (neighbour->isAvailable(_unit->getType())) {
			return neighbour;
		}
	}
	//Left
	neighbourId = thisTileId - 1;
	if (!wouldCloseTileCrossBorder(neighbourId)) {
		Tile* neighbour = _mapP->getTilesP()[neighbourId];
		if (neighbour->isAvailable(_unit->getType())) {
			return neighbour;
		}
	}
	//Right
	neighbourId = thisTileId + 1;
	if (!wouldCloseTileCrossBorder(neighbourId)) {
		Tile* neighbour = _mapP->getTilesP()[neighbourId];
		if (neighbour->isAvailable(_unit->getType())) {
			return neighbour;
		}
	}
	//Down
	neighbourId = thisTileId + _mapP->getColumns();
	if (!wouldCloseTileCrossBorder(neighbourId)) {
		Tile* neighbour = _mapP->getTilesP()[neighbourId];
		if (neighbour->isAvailable(_unit->getType())) {
			return neighbour;
		}
	}
	//Up + left
	neighbourId = thisTileId - _mapP->getColumns() - 1;
	if (!wouldCloseTileCrossBorder(neighbourId)) {
		Tile* neighbour = _mapP->getTilesP()[neighbourId];
		if (neighbour->isAvailable(_unit->getType())) {
			return neighbour;
		}
	}
	//Up + right
	neighbourId = thisTileId - _mapP->getColumns() + 1;
	if (!wouldCloseTileCrossBorder(neighbourId)) {
		Tile* neighbour = _mapP->getTilesP()[neighbourId];
		if (neighbour->isAvailable(_unit->getType())) {
			return neighbour;
		}
	}
	//Down + left
	neighbourId = thisTileId + _mapP->getColumns() - 1;
	if (!wouldCloseTileCrossBorder(neighbourId)) {
		Tile* neighbour = _mapP->getTilesP()[neighbourId];
		if (neighbour->isAvailable(_unit->getType())) {
			return neighbour;
		}
	}
	//Down + right
	neighbourId = thisTileId + _mapP->getColumns() + 1;
	if (!wouldCloseTileCrossBorder(neighbourId)) {
		Tile* neighbour = _mapP->getTilesP()[neighbourId];
		if (neighbour->isAvailable(_unit->getType())) {
			return neighbour;
		}
	}

	return nullptr;
}

void Engine::setPointersToThisUnit(Tile* nextTile) {
	//This tile
	if (_unit->getType() == Unit::Type::LAND) {
		_unit->getCurrentTileP()->setLandUnitP(nullptr);
	}
	else {
		_unit->getCurrentTileP()->setAirUnitP(nullptr);
	}

	//Next tile
	if (_unit->getType() == Unit::Type::LAND) {
		nextTile->setLandUnitP(_unit);
	}
	else {
		nextTile->setAirUnitP(_unit);
	}
}

void Engine::move() {
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

int Engine::getGroupId(bool isNew) {
	//Use isNew = false for standard calls of this function
	if (isNew) {
		return _groupIdNew;
	}
	else {
		return _groupId;
	}
}

Tile* Engine::getIntentedNextTile() {
	return _intendedNextTile;
}

std::stack<Tile*>* Engine::getPathP() {
	return &_path;
}

bool Engine::getWantsToMove() {
	return _wantsToMove;
}

bool Engine::getMoving() {
	return _moving;
}

bool Engine::getHasHigherPriority() {
	return _hasHigherPriority;
}

int Engine::getDistance() {
	return _distance;
}

void Engine::setGroupId(int groupId, bool isFromOtherThread) {
	if (isFromOtherThread) {
		_groupIdNew = groupId;
		_shouldUpdateGroupIdNew = true;
	}
	else {
		_groupId = groupId;
	}
}

void Engine::setPath(std::stack<Tile*> path, bool isFromOtherThread) {
	if (isFromOtherThread) {
		_pathNew = path;
		_shouldUpdatePath = true;
	}
	else {
		_path = path;
	}
}

void Engine::setLeadersPathRelativeIdChange(std::stack<int> path, bool isFromOtherThread) {
	if (isFromOtherThread) {
		_leadersPathRelativeIdChangeNew = path;
		_shouldUpdateLeadersPathRelativeIdChange = true;
	}
	else {
		_leadersPathRelativeIdChange = path;
	}
}

void Engine::setFollowingLeader(bool followingLeader, bool isFromOtherThread) {
	if (isFromOtherThread) {
		_followingLeaderNew = followingLeader;
		_shouldUpdateFollowingLeaderNew = true;
	}
	else {
		_followingLeader = followingLeader;
	}
}

void Engine::setWantsToMove(bool wantsToMove, bool isFromOtherThread) {
	if (isFromOtherThread) {
		_wantsToMoveNew = wantsToMove;
		_shouldUpdateWantsToMoveNew = true;
	}
	else {
		_wantsToMove = wantsToMove;
	}
}

void Engine::setMoving(bool moving) {
	_moving = moving;
}

void Engine::setHasHigherPriority(bool hasHigherPriority) {
	_hasHigherPriority = hasHigherPriority;
}

void Engine::setDistance(int distance) {
	_distance = distance;
}