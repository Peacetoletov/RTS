#ifndef UNIT_H
#define UNIT_H

#include <stack>
#include <vector>

class Tile;
class Pathfinder;

class Unit {
public:
	enum Type {
		LAND,
		AIR
	};

	Unit();		//Won't be used

	Unit(Tile* currentTile, Tile** tiles, Type type, std::vector<Unit*>* unitsP, Pathfinder* pathfinderP);

	~Unit();		//Not used yet

	void update();

	//Getters
	int getGroupId();
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
	void setGroupId(int groupId);
	void setPath(std::stack<Tile*> path);
	void setLeadersPathRelativeIdChange(std::stack<int> path);
	void setFollowingLeader(bool followingLeader);
	void setWantsToMove(bool wantsToMove);
	void setMoving(bool moving);
	void setDistance(int distance);
	void setHovered(bool hovered);
	void setSelected(bool selected);

private:
	int _groupId = -1;				//-1 if the unit isn't in any group; 0-99 if it is in one.
	Tile* _currentTileP;
	Tile** _tiles;
	Type _type;
	std::vector<Unit*>* _unitsP;			//vector of all units on the map
	Pathfinder* _pathfinderP;
	float _speed = 0.25f;					//0.25
	std::stack<Tile*> _path;
	std::stack<int> _leadersPathRelativeIdChange;
	bool _followingLeader;
	bool _wantsToMove = false;
	bool _moving = false;
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
	Tile* chooseNextTile();
	void setPointersToThisUnit(Tile* nextTile);
	void move();
	void avoidDynamicObstacle();
};

#endif