#ifndef UNIT_H
#define UNIT_H

#include <stack>

class Tile;

class Unit {
public:
	enum Type {
		LAND,
		AIR
	};

	Unit();		//Won't be used

	Unit(Tile* currentTile, Type type);

	~Unit();		//Not used yet

	/* TODO
	I need to fix an issue that happens when 2 units cross their paths and one unit needs to wait for the other one.
	I will create bool variables "wantsToMove" and "waiting".
	*/
	void move();

	//Getters
	Type getType();
	Tile* getCurrentTileP();
	std::stack<Tile*>* getPathP();
	bool getWantsToMove();
	bool getMoving();
	int getDistance();

	//Setters
	//void setCurrentTileP(int id);			//is this even needed?		//I'm pretty sure it is now
	void setPath(std::stack<Tile*> path);
	void setWantsToMove(bool wantsToMove);
	void setMoving(bool moving);
	void setDistance(int distance);

private:
	Type _type;
	Tile* _currentTileP;
	float _speed = 1;
	std::stack<Tile*> _path;
	bool _wantsToMove = false;
	bool _moving = false;

	/* This represents the imaginary distance (NOT in pixels) between the current tile 
	and the tile where the unit wants to travel.
	*/
	int _distance;
	//I'll also implement int currentDistance
};

#endif