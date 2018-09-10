#include "map.h"
#include "tile.h"
#include "unit.h"
#include "globals.h"
#include "pathfinder.h"

#include <iostream>

/* class Map
This class contains information about the tiles on the map.
*/

Map::Map() {}

Map::Map(int rows, int columns, Pathfinder* pathfinderP) : 
	_rows(rows),
	_columns(columns),
	_pathfinderP(pathfinderP)
{
	//Create tiles
	int tilesAmount = rows * columns;
	Tile** tiles = new Tile*[tilesAmount];

	for (int id = 0; id < tilesAmount; id++) {
		tiles[id] = new Tile(id, Tile::TerrainAvailability::ALL, this);
	}

	_tiles = tiles;

	//Set neighbours to each tile
	for (int id = 0; id < tilesAmount; id++) {
		this->setNeighbours(id, tiles);
	}	
}

Map::~Map() {
	//Delete Units in _units
	for (int i = 0; i < this->_units.size(); i++) {
		delete _units[i];
	}

	//TODO: delete Building in _buildings

	//Delete contents of array _tiles and the array itself
	int tilesAmount = _rows * _columns;
	for (int i = 0; i < tilesAmount; i++) {
		delete _tiles[i];
	}
	delete[] _tiles;

	//Delete constant booleans truePtr and falsePtr
	delete _truePtr;
	delete _falsePtr;
}

void Map::loadTestMap() {
	/* Legend 
	# = ALL
	y = AIR
	x = NONE
	*/

	std::vector<std::string> mapVector;
	/*
	mapVector = { 
		"##y###",
		"######",
		"######",
		"######",
		"###x##",
	};
	*/
	/* 10 x 8
	mapVector = {
		"#yyy####",
		"###y####",
		"###yxxxx",
		"###yx###",
		"#yxyx###",
		"####x###",
		"####x###",
		"####x###",
		"####x###",
		"######yy",
	};
	*/
	
	//100 x 80
	mapVector = {
		"#########################xxxxxxxxxxxxxxxxxxxx#######################################################",
		"####################################################################################################",
		"#########################xxxxxxxxxxxxxxxxxxxx#######################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"xxxxxxxxxx######xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx#####xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx#xxxxxxxxx",
		"###x################################################################################################",
		"###x################################################################################################",
		"###x################################################################################################",
		"xxxx################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################y##yyy##yy######y##y#y#y###y#y##################################################",
		"###################y#y##y###y#y####y#y#y#y#y###y#y##################################################",
		"###################yyy##y###yy#####y#y#y#y#y####y###################################################",
		"###################y#y##y###yy#####y#y#yyy#y####y###################################################",
		"###################y#y#yyy##y#y#####y##y#y#yyy##y###################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"########################################xxxxxxxxxxxxxxxxxx##########################################",
		"###############################################x####################################################",
		"###############################################x####################################################",
		"###############################################x####################################################",
		"#####################xxxxxxxxxxxxxxxx##########x####################################################",
		"###########################x###################x####################################################",
		"###############################################x####################################################",
		"###########################x###################x####################################################",
		"###########################x###################x####################################################",
		"###############################################x####################################################",
		"xxxxx#####xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx#####xxxxxxxxxxxxxxxxxxxxxxxxx",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx##xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx#####xxxxxxxx",
		"###x################################################################################################",
		"###x################################################################################################",
		"###x################################################################################################",
		"xxxx################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################",
		"####################################################################################################"
	};
	

	
	//50 x 40
	/*
	mapVector = {
		"##################################################",
		"##################################################",
		"##################################################",
		"##################################################",
		"xxxxxxxxxxxxxxxxxxxxxxx#xxxxxxxxxxxxxxxxxx#xxxxxxx",
		"##########x#######################################",
		"##########x#######################################",
		"##########x#######################################",
		"##########x#######################################",
		"##################################################",
		"##################################################",
		"##########x#######################################",
		"##########x#######################################",
		"##################################################",
		"##########x#######x###############################",
		"##########x#######x###############################",
		"##########x#######x###############################",
		"##########x#######x###############################",
		"##########xxxxxxxx################################",
		"##################################################",
		"##################################################",
		"##################################################",
		"##################################################",
		"x#xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx#",
		"##################################################",
		"##################################################",
		"##################################################",
		"###################x##############################",
		"###################x##############################",
		"###################x##############################",
		"###################x##############################",
		"############xxxxxxxxxxxxxxx#######################",
		"###################x##############################",
		"###################x##############################",
		"###################x##############################",
		"##################################################",
		"##################################################",
		"##################xxx##########################xxx",
		"###############################################x#x",
		"###############################################xxx",
	};
	*/
	

	/*
	//25 x 20
	mapVector = {
		"#########################",
		"#########################",
		"#########################",
		"#########################",
		"#########################",
		"#########################",
		"#########################",
		"#########################",
		"#####yyyyyyxxxxxxxxxx####",
		"#########################",
		"#########################",
		"#########################",
		"#########################",
		"#########################",
		"#########################",
		"#########################",
		"#xxxxxxxxxxxxxxxxxxxxxxx#",
		"#########################",
		"#########################",
		"#########################",
	};
	*/

	for (int id = 0; id < (_rows * _columns); id++) {
		if (mapVector[idToRow(id)][idToColumn(id)] == '#') {
			this->_tiles[id]->setTerrainType(Tile::TerrainAvailability::ALL);
		}
		else if (mapVector[idToRow(id)][idToColumn(id)] == 'y') {
			//cout << "Setting air tile to row " << idToRow(id) << " and column " << idToColumn(id) << endl;
			this->_tiles[id]->setTerrainType(Tile::TerrainAvailability::AIR);
		}
		else if (mapVector[idToRow(id)][idToColumn(id)] == 'x') {
			this->_tiles[id]->setTerrainType(Tile::TerrainAvailability::NONE);
		}
		else {
			std::cout << "Error in creating a map" << std::endl;
		}
	}

	/*
	std::cout << "Position of [0][0] is " << this->_terrain[0][0] << endl;
	std::cout << "Position of [0][2] is " << this->_terrain[0][2] << endl;
	std::cout << "Position of [4][3] is " << this->_terrain[4][3] << endl;
	*/
	
}

void Map::loadTestUnits(const int amount, int row[], int column[], Unit::Type type[]) {
	for (int i = 0; i < amount; i++) {
		Tile* tile = _tiles[positionToId(row[i], column[i])];
		Unit* unit = new Unit(tile, _tiles, type[i], _pathfinderP, this);
		if (type[i] == Unit::Type::LAND) {
			tile->setLandUnitP(unit);
		}
		else {
			tile->setAirUnitP(unit);
		}
		_units.push_back(unit);
	}
	
	
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

void Map::update() {
	//UNITS
	/* Whenever I'm moving a unit, I need to change the occupancy of both the tile it left, and the tile it's now on.
	If a stationary unit gets a command to move, it first changes its wantsToMove variable to true.
	Then it check the next tile on the path. If the unit can move there, its variable moving is set to true.
	Once it gets to the target location, wantsToMove and moving get set to false.
	If it encounters a moving obstacle and needs to stop because of that, moving is set to false but
	wantsToMove still remains true.
	*/

	/* If this gets too inefficient, I can make it so that I only check every 5th frame or something like this.
	*/
	for (int i = 0; i < _units.size(); i++) {
		_units[i]->update();
	}
}

void Map::setSelectedUnits(std::vector<Unit*> selectedUnits) {
	_selectedUnits = selectedUnits;
}

int Map::getRows() {
	return this->_rows;
}

int Map::getColumns() {
	return this->_columns;
}

std::vector<Unit*>* Map::getUnitsP() {
	return &this->_units;
}

std::vector<Unit*>* Map::getSelectedUnitsP() {
	return &_selectedUnits;
}

Tile** Map::getTilesP() {
	return this->_tiles;
}

//PRIVATE METHODS
void Map::setNeighbours(int id, Tile** tiles) {
	/*
	std::vector<Tile*> neighbours;
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
	*/

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

			//Add this tile to current tile's neighbours vector
			if ((r == -1 || r == 1) && (c == -1 || c == 1)) {
				//Diagonal
				tiles[id]->setNeighbour(tiles[positionToId(row + r, column + c)], _truePtr);
			}
			else {
				//Straight
				tiles[id]->setNeighbour(tiles[positionToId(row + r, column + c)], _falsePtr);
			}
			
		}
	}
}