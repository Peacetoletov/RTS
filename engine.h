#pragma once

#ifndef ENGINE_H
#define ENGINE_H

#include <stack>

class Unit;
class Pathfinder;
class Map;
class Tile;

class Engine {
public:
	Engine();		//Won't be used

	Engine(Unit* unit, Pathfinder* pathfinderP, Map* mapP);

	//Main function
	void update();

	//Getters
	int getGroupId(bool isNew);
	Tile* getIntentedNextTile();
	std::stack<Tile*>* getPathP();
	bool getWantsToMove();
	bool getMoving();
	bool getHasHigherPriority();
	int getDistance();
	bool getHovered();
	bool getSelected();

	//Setters
	void setGroupId(int groupId, bool isFromOtherThread);
	void setPath(std::stack<Tile*> path, bool isFromOtherThread);
	void setLeadersPathRelativeIdChange(std::stack<int> path, bool isFromOtherThread);
	void setFollowingLeader(bool followingLeader, bool isFromOtherThread);
	void setWantsToMove(bool wantsToMove, bool isFromOtherThread);
	void setMoving(bool moving);
	void setHasHigherPriority(bool isMakingRoomForOtherUnit);
	void setDistance(int distance);
	void setHovered(bool hovered);
	void setSelected(bool selected);

private:
	Unit* _unit;
	Pathfinder* _pathfinderP;
	Map* _mapP;
	int _groupId = -1;							//-1 if the unit isn't in any group; 0-99 if it is in one.
	float _speed = 0.25f;						//0.25 for debugging, 1.25 for fast, 2.25 for supersonic
	Tile* _intendedNextTile;					//What tile this unit wants to go to
	std::stack<Tile*> _path;
	std::stack<int> _leadersPathRelativeIdChange;
	bool _followingLeader;
	bool _wantsToMove = false;
	bool _moving = false;

	/* A counter that goes up by 1 each frame the unit is being blocked by a stationary unit (which can be wanting to move, but
	unable to) while following the leader. When the counter reaches a certain threshold (10 frames?) and the blocking unit
	is still there, this unit will stop following the leader. The reason for having the counter in the first place is because
	the part of code that triggers it can happen for 1 frame even in normal scenarios - e. g. when a big, clustered group of
	units is ordered to move. When the unit successfuly moves, the counter resets.
	*/
	int _shouldStopFollowingLeaderCounter;
	const int _shouldStopFollowingLeaderThreshold = 10;		//The amount of frames before the unit stops wanting to move.

	/* A counter that goes up by 1 each frame the unit is being blocked by a stationary unit while following the vector field.
	When the counter reaches a certain threshold (10 frames?) and the blocking unit is still there, unwilling to move,
	this unit will decide to avoid the obstacle by going 45 degrees from the original path, if possible. If it isn't
	possible because both tiles 45 degrees from the original next tile are occupied, the chooseNextTile() method will
	return nullptr, meaning the unit will stop trying to move.
	*/
	int _shouldTryToAvoidStationaryObstacleCounter;
	const int _shouldTryToAvoidStationaryObstacleThreshold = 10;		//The amount of frames before the unit tried to avoid the obstacle

	/* Used in avoidDynamicObstacle(). When 2 units are blocking each other, the first one to reach that code will switch this
	variable to false and will have the priority, meaning that the other unit is the one to make room for this unit. The other
	unit will see that this variable is false and won't try to make take the priority as well, and instead will make room.
	*/
	bool _hasHigherPriority = false;

	/* Used in avoidDynamicObstacle(). If 2 units are blocking each other and one of them is following the vector field, I can't
	just add 2 additional tiles to the stack that the unit is taking its path tiles from, because a unit that is following the
	vector field doesn't have any stack of that kind. So when this situation happens, the next call of the function
	getNextTileIfFollowingVectorField() will return this tile instead of the parent tile.
	*/
	Tile* _higherPriorityTileInVectorField;

	/* These variables are shared between 2 threads. To avoid overriding one while I use it in a function, I will instead
	save the values received from the other thread here. When I need the values in a function, I will update them. The point
	is that the values I will work with will not be updated in the middle of a function.
	These variables are write-only except for the part at the start of the update() function where I move the values from
	these variables into the normal variables.
	*/
	std::stack<Tile*> _pathNew;
	std::stack<int> _leadersPathRelativeIdChangeNew;
	bool _followingLeaderNew;
	bool _wantsToMoveNew;
	int _groupIdNew;
	/* I need to know whether or not I need to update the variables. Since I cannot assign NULL to a stack, I need
	these helper variables to determine it.
	*/
	bool _shouldUpdatePath;
	bool _shouldUpdateLeadersPathRelativeIdChange;
	bool _shouldUpdateFollowingLeaderNew;
	bool _shouldUpdateWantsToMoveNew;
	bool _shouldUpdateGroupIdNew;

	/* This represents the imaginary distance (NOT in pixels) between the current tile
	and the tile where the unit wants to travel.
	*/
	float _distance;
	/* _currentDistance represents how close to the tile that the unit is moving onto is.
	It is always less than _distance and once it overflows, it gets reset back to 0.
	At that moment, the unit is fully on the new tile and if it's not at the end of
	the path yet, it moves onto the next tile in the _path stack.
	I can also look at this variable as the distance from the previous tile.
	*/
	float _currentDistance;

	//METHODS
	void updateVariables();
	Tile* chooseNextTile();
	Tile* getNextTileIfFollowingVectorField();
	Tile* getNextTileIfFollowingLeader();
	bool wouldTileBeOutOfBounds(int tileId);

	/* When following the leader, if going the in same direction as the leader would result in going to a tile that
	has a huge difference between the tile's parent vector and the leader's relative vector, returns true.
	*/
	bool wouldFollowingLeaderResultInWrongDirection(Tile* untestedNextTile);

	//TODO: NEEDS DOCUMENTATION
	Tile* tryToFindCloseAvailableTile();
	bool wouldCloseTileCrossBorder(int tileId);
	bool canMoveToNextTile(Tile* nextTile);
	void setPointersToThisUnit(Tile* nextTile);
	void move();
	void avoidDynamicObstacle();
	Tile* getAnyAvailableNeighbourTile();
};

#endif