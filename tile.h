#ifndef TILE_H
#define TILE_H

#include <vector>

class Map;

class Tile {
public:
	enum TerrainAvailability {
		ALL,
		AIR,
		NONE
	};

	Tile();			//Won't be used

	/* Tile
	Creates the object and initializes _id, _type, _mapP.
	*/
	Tile(int id, TerrainAvailability type, Map* mapP);

	/* ~Tile
	Test purpose only (for now)
	*/
	~Tile();

	void test();		//Test purpose only

	/* int calculateH
	Calculates H (the distance between this tile and the end tile).
	*/
	int calculateH(Tile* endTile);

	//Setters
	void setType(TerrainAvailability type);
	void setNeighbours(std::vector<Tile*> neighbours);
	void setWasChecked(bool wasChecked);
	void setG(int G);
	void setH(int H);

	//Getters
	int getId();
	TerrainAvailability getType();
	bool getWasChecked();
	//int getG();
	//int getH();		//I'm not sure if I need getG and getH
	int getF();

private:
	int _id;
	TerrainAvailability _type;
	Map* _mapP;							//Pointer to the map object, allows the use of utility functions
										//(switching between columns and rows and id)
	std::vector<Tile*> _neighbours;		//Vector holding pointers to neighbour tiles
	bool _wasChecked;
	int _G;								//Distance from start
	int _H;								//Minimal distance to end
	//int _F doesn't have to be stored, as I can get it by summing up _G and _H

};

#endif