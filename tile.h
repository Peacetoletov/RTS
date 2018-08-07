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

	//Which units are currently standing on this tile
	enum class Occupancy {
		LAND,
		AIR,		
		LAND_AND_AIR,
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

	/* bool isNeighbourDiagonal
	Returns true if the neighbour tile is touching this tile by a corner.
	Returns false if the neighbour tile is touching this tile by an edge.
	*/
	bool isNeighbourDiagonal(Tile* neighbour);

	//Resets all member variables to their default state.
	void reset();

	/* Determines (based on TerrainAvailability and Occupancy) whether this tile is available 
	for a unit of the specified type.
	*/
	bool isAvailable(Unit::Type unitType);

	//Setters
	void setTerrainType(TerrainAvailability terrainType);
	void setOccupancy(Occupancy occupancy);
	void setDirection(Direction direction);
	void setNeighbours(std::vector<Tile*> neighbours);
	void setWasVisited(bool wasVisited);
	void setG(int G);
	void setH(int H);
	void setParentP(Tile* parentP);

	//Getters
	int getId();
	TerrainAvailability getTerrainType();
	Occupancy getOccupancy();
	Direction getDirection();
	std::vector<Tile*>* getNeighboursP();
	bool getWasVisited();
	int getG();		
	int getH();			//This one is currently being used for testing
	int getF();
	Tile* getParentP();

private:
	int _id;
	TerrainAvailability _terrainType;
	Occupancy _occupancy = Occupancy::NONE;
	Direction _direction;
	Map* _mapP;							//Pointer to the map object, allows the use of utility functions
										//(switching between columns and rows and id)
	std::vector<Tile*> _neighbours;		//Vector holding pointers to neighbour tiles
	bool _wasVisited;
	int _G;								//Distance from start
	int _H;								//Minimal distance to end
	//int _F doesn't have to be stored, as I can get it by summing up _G and _H
	Tile* _parentP;

};

#endif