#include "map.h"

#include <iostream>

/* class Map
This class contains information about the tiles on the map.
*/

Map::Map() {}

Map::Map(int rows, int columns) :
	_terrain(rows, vector<TerrainAvailability>(columns, ALL))	//_terrain[row][column]
{
	
}

void Map::loadTestMap() {
	/* Legend 
	# = ALL
	y = AIR
	x = NONE
	*/

	vector<string> mapVector;
	mapVector = { 
		"##y###",
		"######",
		"######",
		"######",
		"###x##",
	};

	for (int i = 0; i < mapVector.size(); i++) {
		for (int j = 0; j < mapVector[0].size(); j++) {
			if (mapVector[i][j] == '#') {
				setTerrainTile(i, j, Map::ALL);
			}
			else if (mapVector[i][j] == 'y') {
				cout << "Setting air tile to [" << i << "][" << j << "]" << endl;
				setTerrainTile(i, j, Map::AIR);
			}
			else if (mapVector[i][j] == 'x') {
				setTerrainTile(i, j, Map::NONE);
			}
			else {
				std::cout << "Error in creating a map" << endl;
			}
		}
	}
	
	/*
	std::cout << "Position of [0][0] is " << this->_terrain[0][0] << endl;
	std::cout << "Position of [0][2] is " << this->_terrain[0][2] << endl;
	std::cout << "Position of [4][3] is " << this->_terrain[4][3] << endl;
	*/
	
}

vector<vector<Map::TerrainAvailability> > * Map::getTerrainP() {
	return &this->_terrain;
}

void Map::setTerrainTile(int row, int column, TerrainAvailability type) {
	this->_terrain[row][column] = type;
}