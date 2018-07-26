#ifndef TILE_H
#define TILE_H

#include <vector>

class Map;

class Tile {
public:
	//Which units can stand on this tile
	enum TerrainAvailability {
		ALL,
		AIR,
		NONE
	};

	//Which units are currently standing on this tile
	enum Occupancy {
		LAND,
		AIR,
		LAND_AND_AIR,
		NONE
	};

	Tile();			//Won't be used

	//Creates the object and initializes _id, _type, _mapP.
	Tile(int id, TerrainAvailability type, Map* mapP);

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

	//Setters
	void setTerrainType(TerrainAvailability type);
	void setOccupancy(Occupancy occupancy);
	void setNeighbours(std::vector<Tile*> neighbours);
	void setWasVisited(bool wasVisited);
	void setG(int G);
	void setH(int H);
	void setParentP(Tile* parentP);

	//Getters
	int getId();
	TerrainAvailability getTerrainType();
	Occupancy getOccupancy();
	std::vector<Tile*>* getNeighboursP();
	bool getWasVisited();
	int getG();		
	int getH();			//This one is currently being used for testing
	int getF();
	Tile* getParentP();

private:
	int _id;
	TerrainAvailability _type;
	Occupancy _occupancy;
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