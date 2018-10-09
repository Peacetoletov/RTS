#ifndef UNIT_H
#define UNIT_H

#include <stack>
#include <vector>

class Tile;
class Pathfinder;
class Map;
class Engine;

class Unit {
public:
	enum Type {
		LAND,
		AIR
	};

	Unit();		//Won't be used

	Unit(Tile* currentTile, Type type, Pathfinder* pathfinderP, Map* mapP);

	~Unit();		//Not used yet

	//Main function
	void update();

	//Getters
	Engine* getEngineP();
	Tile* getCurrentTileP();
	Type getType();

	//Setters
	void setCurrentTileP(Tile* currentTileP);

private:
	Engine* _engine;
	Tile* _currentTileP;
	Map* _mapP;
	Type _type;

	bool _hovered = false;
	bool _selected = false;

	
};

#endif