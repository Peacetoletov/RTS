#include "map.h"

#include <iostream>

/* class Map
This class contains information about the tiles on the map.
*/

Map::Map() {}

Map::Map(int rows, int columns) :
		_terrain(rows, vector<TerrainAvailability>(columns, AIR))
{
	
	//TODO: Fix pathfinder (some problems with pointers)


	cout << "Creating vector! " << this->_terrain[0][0] << endl;
	cout << "Size of vector in map = " << this->_terrain.size() << endl;
}

void Map::setTerrainTile(int row, int column, TerrainAvailability type) {
	_terrain[row][column] = type;		
}

vector<vector<TerrainAvailability> > Map::getTerrain() {
	return _terrain;
}