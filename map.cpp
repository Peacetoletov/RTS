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

	for (int id = 0; id < tilesAmount; id++) {
		tiles[id] = new Tile(id, Tile::TerrainAvailability::ALL, this);
	}

	this->_tiles = tiles;

	//Set neighbours to each tile
	for (int id = 0; id < tilesAmount; id++) {
		this->setNeighbours(id, tiles);
	}
	
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
		"#yyy####",
		"########",
		"###yx###",
		"####x###",
		"####x###",
		"####x###",
		"####x###",
		"####x###",
		"####x###",
		"######yy",
	};

	for (int id = 0; id < (_rows * _columns); id++) {
		if (mapVector[idToRow(id)][idToColumn(id)] == '#') {
			this->_tiles[id]->setType(Tile::ALL);
		}
		else if (mapVector[idToRow(id)][idToColumn(id)] == 'y') {
			//cout << "Setting air tile to row " << idToRow(id) << " and column " << idToColumn(id) << endl;
			this->_tiles[id]->setType(Tile::AIR);
		}
		else if (mapVector[idToRow(id)][idToColumn(id)] == 'x') {
			this->_tiles[id]->setType(Tile::NONE);
		}
		else {
			std::cout << "Error in creating a map" << endl;
		}
	}

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

Tile** Map::getTilesP() {
	return this->_tiles;
}

//PRIVATE METHODS
void Map::setNeighbours(int id, Tile** tiles) {
	vector<Tile*> neighbours;
	int row = idToRow(id);
	int column = idToColumn(id);

	//Loop through all tiles around the current tile
	for (int r = -1; r <= 1; r++) {	
		for (int c = -1; c <= 1; c++) {

			//Skip iterating through itself
			if (r == 0 && c == 0) {
				continue;
			}

			//If the tile is at the edge, skip tiles that would be out of bounds
			if ((row == 0 && r == -1) || 
				(row == (this->_rows - 1) && r == 1) || 
				(column == 0 && c == -1) || 
				(column == (this->_columns - 1) && c == 1)) {
				continue;
			}

			//Add this tile to the neighbours vector
			neighbours.push_back(tiles[positionToId(row + r, column + c)]);
		}
	}

	//Add the vector to the current tile
	tiles[id]->setNeighbours(neighbours);
}