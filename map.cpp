#include "map.h"

#include <iostream>

/* class Map
This class contains information about the tiles on the map.
*/

Map::Map() {}

Map::Map(int rows, int columns) : 
	_rows(rows),
	_columns(columns)
{
	//Create tiles
	int tilesAmount = rows * columns;
	Tile** tiles = new Tile*[tilesAmount];

	for (int i = 0; i < tilesAmount; i++) {
		tiles[i] = new Tile(i, Tile::TerrainAvailability::ALL);
	}

	this->_tiles = tiles;
}

Map::~Map() {
	//Delete GameObjects in _objects
	for (int i = 0; i < this->_objects.size(); i++) {
		delete this->_objects[i];
	}

	//Delete contents of array _tiles and the array itself
	int tilesAmount = _rows * _columns;
	for (int i = 0; i < tilesAmount; i++) {
		delete this->_tiles[i];
	}
	delete[] this->_tiles;
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

	//TODO: Make this work again

	/*
	for (int i = 0; i < mapVector.size(); i++) {
		for (int j = 0; j < mapVector[0].size(); j++) {
			if (mapVector[i][j] == '#') {
				setTerrainTile(i, j, Tile::ALL);
			}
			else if (mapVector[i][j] == 'y') {
				cout << "Setting air tile to [" << i << "][" << j << "]" << endl;
				setTerrainTile(i, j, Tile::AIR);
			}
			else if (mapVector[i][j] == 'x') {
				setTerrainTile(i, j, Tile::NONE);
			}
			else {
				std::cout << "Error in creating a map" << endl;
			}
		}
	}
	*/
	
	/*
	std::cout << "Position of [0][0] is " << this->_terrain[0][0] << endl;
	std::cout << "Position of [0][2] is " << this->_terrain[0][2] << endl;
	std::cout << "Position of [4][3] is " << this->_terrain[4][3] << endl;
	*/
	
}

void Map::loadTestObjects() {
	//Currently, I'm only creating 1 unit at row 4, column 1
	GameObject* unit = new GameObject(positionToId(4, 1));
	this->_objects.push_back(unit);
}

int Map::idToRow(int id) {
	return (id / this->_columns);
}

int Map::idToColumn(int id) {
	return (id % this->_columns);
}

int Map::positionToId(int row, int column) {
	return (column + (row * this->_columns));
}

int Map::getRows() {
	return this->_rows;
}

int Map::getColumns() {
	return this->_columns;
}

vector<GameObject*>* Map::getObjectsP() {
	return &this->_objects;
}