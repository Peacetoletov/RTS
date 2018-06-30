#ifndef MAP_H
#define MAP_H

#include <vector>

using namespace std;

/* enum TerrainAvailability
Indicates which units can move to the tile of that type.
*/

enum TerrainAvailability {
	ALL,
	AIR,
	NONE
};

/* class Map
This class contains information about the tiles on the map.
*/

class Map {
public:
	Map();

	/* Map
	Initializes a map with all tiles available to all units (empty map).
	*/
	Map(int rows, int columns);

	//TODO: Create a constructor that take an aditional parameter to immediately create all tiles.

	/* void setTerrainTile
	Sets a tile in the map to a certain type, based on the terrain availability.
	*/
	void setTerrainTile(int row, int column, TerrainAvailability type);

	/* vector<vector<TerrainAvailability> > getTerrain
	Returns terrain.
	*/
	vector<vector<TerrainAvailability> > * getTerrainP();
	//int getTerrain();

private:
	vector<vector<TerrainAvailability> > _terrain;
	//int _terrain;	//test
};

#endif