#include "map.h"

#include <iostream>

/* class Map
This class contains information about the tiles on the map.
*/

Map::Map() {}

Map::Map(int rows, int columns) :
	//TODO: Remove _terrain completely
	_terrain(rows, vector<TerrainAvailability>(columns, ALL))	//_terrain[row][column]
{
	//Create tiles
	int tilesAmount = rows * columns;
	Tile** tiles = new Tile*[tilesAmount];

	for (int i = 0; i < tilesAmount; i++) {
		//tiles[i] = new Tile(i);
	}

	/* TODO
	Instead of storing 2 variables in each tile (row, line), I will only store 1 variable (id).
	I will also store 2 variables (rows, lines) in Map, passed in constructor from Level.
	This will allow for easier manipulation with tiles as a whole. 
	A simple function can be used to convert the id to a row or column, thanks to the presence
	of variables rows and lines, containing the amount of rows and lines a level has.
	*/

	//TODO: Delete tiles!

}

Map::~Map() {
	for (int i = 0; i < this->_objects.size(); i++) {
		delete this->_objects[i];
	}
}

void Map::loadTestMap() {
	/* Legend 
	# = ALL
	y = AIR
	x = NONE
	*/

	vector<string> mapVector;
	/*
	mapVector = { 
		"##y###",
		"######",
		"######",
		"######",
		"###x##",
	};
	*/
	mapVector = {
		"########",
		"########",
		"####x###",
		"####x###",
		"####x###",
		"####x###",
		"####x###",
		"####x###",
		"####x###",
		"########",
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

void Map::loadTestObjects() {
	//Currently, I'm only creating 1 unit at [1][4]
	GameObject* unit = new GameObject(1, 4);
	this->_objects.push_back(unit);
}

vector<vector<Map::TerrainAvailability> >* Map::getTerrainP() {
	return &this->_terrain;
}

vector<GameObject*>* Map::getObjectsP() {
	return &this->_objects;
}

void Map::setTerrainTile(int row, int column, TerrainAvailability type) {
	this->_terrain[row][column] = type;
}