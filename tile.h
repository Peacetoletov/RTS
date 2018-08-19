#ifndef TILE_H
#define TILE_H

#include <vector>

#include "unit.h"			//necessary because of Unit::Type

class Map;

class Tile {
public:
	//Which units can stand on this tile
	enum class TerrainAvailability {
		ALL,
		AIR,
		NONE
	};

	/* This is used in the bidirectional Dijkstra pathfinding algortihm.
	I have 2 directions so this is used to distinct which direction the tile belongs to.
	*/
	enum class Direction {
		START,
		TARGET,
		NONE
	};

	Tile();			//Won't be used

	//Creates the object and initializes _id, _terrainType, _mapP.
	Tile(int id, TerrainAvailability terrainType, Map* mapP);

	//Test purpose only (for now)
	~Tile();

	void test();		//Test purpose only

	//Calculates H (the distance between this tile and the end tile).
	int calculateH(Tile* endTile);

	/* Returns true if the neighbour tile is touching this tile by a corner.
	Returns false if the neighbour tile is touching this tile by an edge.
	*/
	bool isNeighbourDiagonal(Tile* neighbour);

	//Resets all member variables to their default state.
	void reset();

	/* Determines (based on TerrainAvailability and Occupancy) whether this tile is available 
	for a unit of the specified type.
	*/
	bool isAvailable(Unit::Type unitType);

	/* Similar to isAvailable(Unit::Type unitType), but this one returns true if the tile is occupied
	by a unit that is moving.
	*/
	bool isAvailableForPathfinding(Unit::Type unitType);

	//Setters
	void setTerrainType(TerrainAvailability terrainType);
	void setDirection(Direction direction);
	void setLandUnitP(Unit* unit);
	void setAirUnitP(Unit* unit);
	void setNeighbours(std::vector<Tile*> neighbours);
	void setWasVisited(bool wasVisited);
	void setG(int G);
	void setH(int H);
	void setParentP(Tile* parentP);
	void setGroupParent(Tile* parent, int groupId);

	//Getters
	int getId();
	TerrainAvailability getTerrainType();
	Direction getDirection();
	Unit* getLandUnitP();
	Unit* getAirUnitP();
	std::vector<Tile*>* getNeighboursP();
	bool getWasVisited();
	int getG();		
	int getH();			//This one is currently being used for testing
	int getF();
	Tile* getParentP();
	Tile* getGroupParent(int groupId);		

private:
	int _id;
	TerrainAvailability _terrainType;
	Direction _direction;

	//Which land and air units are on this tile
	Unit* _landUnitP = nullptr;
	Unit* _airUnitP = nullptr;

	//Pointer to the map object, allows the use of utility functions (switching between columns and rows and id)
	Map* _mapP;

	//Pathfinding variables
	std::vector<Tile*> _neighbours;		//Vector holding pointers to neighbour tiles
	bool _wasVisited;
	int _G;								//Distance from start
	int _H;								//Minimal distance to end
	//int _F doesn't have to be stored, as I can get it by summing up _G and _H
	Tile* _parentP;

	/* This is used in group pathfinding. I can store multiple vector fields, one for each group. If I exceed a certain limit (100?),
	I will remove the first field and go from the start. If I use 1/4 of all tiles for 1 group movement, this will take about 800 kb.

	Important: By overwriting the old fields, I may end up with units that don't know where to go.

	To get around this, I could do this: whenever I'm about to overwrite, loop through all units. If they have the group pathfinding
	id of the vector field I'm about to overwrite, I will assign the path the need to take from their point to get to the target,
	essencially just copying parts of the infromation from the vector field to the given unit. Then I can safely overwrite it and
	no unit will get lost.
	*/
	std::vector<Tile*> _groupParent;		//The size is set to 100 in the constructor

};

#endif