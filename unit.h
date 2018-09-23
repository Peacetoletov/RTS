#ifndef UNIT_H
#define UNIT_H

#include <stack>
#include <vector>

class Tile;
class Pathfinder;
class Map;

class Unit {
public:
	enum Type {
		LAND,
		AIR
	};

	Unit();		//Won't be used

	Unit(Tile* currentTile, Type type, Pathfinder* pathfinderP, Map* mapP);

	~Unit();		//Not used yet

	void update();

	//Getters
	int getGroupId(bool isNew);
	Type getType();
	Tile* getCurrentTileP();
	std::stack<Tile*>* getPathP();
	bool getWantsToMove();
	bool getMoving();
	int getDistance();
	bool getHovered();
	bool getSelected();

	//Setters
	//void setCurrentTileP(int id);			//is this even needed?		//I'm pretty sure it is now		//is it though?
	void setGroupId(int groupId, bool isFromOtherThread);
	void setPath(std::stack<Tile*> path, bool isFromOtherThread);
	void setLeadersPathRelativeIdChange(std::stack<int> path, bool isFromOtherThread);
	void setFollowingLeader(bool followingLeader, bool isFromOtherThread);
	void setWantsToMove(bool wantsToMove, bool isFromOtherThread);
	void setMoving(bool moving);
	void setDistance(int distance);
	void setHovered(bool hovered);
	void setSelected(bool selected);

private:
	Tile* _currentTileP;
	Type _type;
	Pathfinder* _pathfinderP;
	Map* _mapP;
	float _speed = 1.25f;					//0.25 for debugging, 1.25 for fast, 2.25 for supersonic
	std::stack<Tile*> _path;
	std::stack<int> _leadersPathRelativeIdChange;
	bool _followingLeader;
	bool _wantsToMove = false;
	int _groupId = -1;				//-1 if the unit isn't in any group; 0-99 if it is in one.
	bool _moving = false;

	/* A counter that goes up by 1 each frame the unit is being blocked by a stationary unit. When the counter reaches 
	a certain threshold (10 frames?) and the blocking unit is still there, unwilling to move, this unit will finally
	decide to stop moving as well.
	*/
	int _shouldStopWantingToMoveCounter;
	const int _shouldStopWantingToMoveCounterThreshold = 10;		//The amount of frames before the unit stops wanting to move.
	//^^ These variables may need to be renamed

	/* This variable tells the unit if it should try to look at the 2 closest tiles in case the parent of the current tile points to
	a tile that isn't available. If it is true, then at least 1 of the 2 closest tiles is available. This variable is set to true
	once the counter reaches the threshold, and is set to false when a new tile is chosen or the unit stops moving because all 3
	closest tiles are occupied.

	Actually, I don't need this extra variable. Instead, I can just check if the counter is equal to the threshold.
	*/
	//bool _shouldTryToAvoidObstacleInVectorField;

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

	bool _hovered = false;
	bool _selected = false;

	//METHODS
	void updateVariables();
	Tile* chooseNextTile();
	bool wouldTileBeOutOfBounds(int tileId);
	Tile* tryToFindCloseAvailableTile();
	bool wouldCloseTileCrossBorder(int tileId);
	bool canMoveToNextTile(Tile* nextTile);
	void setPointersToThisUnit(Tile* nextTile);
	void move();
	void avoidDynamicObstacle();
};

#endif