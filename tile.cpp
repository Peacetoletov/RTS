#include "tile.h"

#include <algorithm>		//std::min, std::max
#include "map.h"		

#include <iostream>

Tile::Tile() {}

Tile::Tile(int id, TerrainAvailability terrainType, Map* mapP) :
	_id(id),
	_terrainType(terrainType),
	_mapP(mapP)
{
	reset();

	/*
	this->_wasVisited = false;
	this->_G = INT_MAX;		//Not infinity, but close enough
	this->_H = INT_MAX;		//Not infinity, but close enough
	*/
}

Tile::~Tile() {
	//std::cout << "Deleting tile " << this->_id << std::endl;
}

void Tile::test() {
	std::cout << "Size of _neighbours vector of tile " << this->_id << " is " << this->_neighbours.size() << std::endl;;
}

int Tile::calculateH(Tile* endTile) {
	//Get difference in rows and columns between the tiles
	int rowDiff = abs(_mapP->idToRow(this->_id) - _mapP->idToRow(endTile->getId()));
	int columnDiff = abs(_mapP->idToColumn(this->_id) - _mapP->idToColumn(endTile->getId()));

	//Determine which one is smaller
	int smaller = std::min(rowDiff, columnDiff);

	//Calculate diagonal distance by taking all units common to both differences (rows and columns)
	//and translating them into the distance, where 1 diagonal has a distance of 14
	//(assuming the length of 1 tile is 10, then 14 is an approximation of sqrt(2 * 10))
	int distance = smaller * 14;

	//By subtracting the smaller value from both differences, one value will become 0 and 
	//the other one will respond to the amount of straight paths.
	distance += (rowDiff + columnDiff - 2 * smaller) * 10;
	
	/*
	std::cout << "Minimal distance between tile " << _mapP->idToRow(this->_id) << "|" << _mapP->idToColumn(this->_id) <<
		" and tile " << _mapP->idToRow(endTile->getId()) << "|" << _mapP->idToColumn(endTile->getId()) << 
		" is " << distance << std::endl;
		*/

	return distance;
}

bool Tile::isNeighbourDiagonal(Tile* neighbour) {
	//Get difference in rows and columns between the tiles
	int rowDiff = abs(_mapP->idToRow(this->_id) - _mapP->idToRow(neighbour->getId()));
	int columnDiff = abs(_mapP->idToColumn(this->_id) - _mapP->idToColumn(neighbour->getId()));

	//If the difference in rows and columns is 1, return true. Otherwise return false.
	return ((rowDiff == 1 && columnDiff == 1) ? true : false);
}

void Tile::reset() {
	_wasVisited = false;
	_G = INT_MAX;				//Not infinity, but close enough
	_H = INT_MAX;				//Not infinity, but close enough
	_parentP = nullptr;
	_direction = Direction::NONE;
}

bool Tile::isAvailable(Unit::Type unitType) {
	//Land
	if (unitType == Unit::Type::LAND) {
		if (_terrainType == TerrainAvailability::ALL) {
			if (_landUnitP == nullptr) {
				return true;
			}
		}
	}
	//Air
	else {
		if (_terrainType == TerrainAvailability::ALL || _terrainType == TerrainAvailability::AIR) {
			if (_airUnitP == nullptr) {
				return true;
			}
		}
	}
	return false;

	/*
	//Land
	if (unitType == Unit::Type::LAND) {
		if (_terrainType == TerrainAvailability::ALL) {
			if (_occupancy == Occupancy::NONE ||
					_occupancy == Occupancy::AIR) {
				return true;
			}
		}	
	}
	//Air
	else {
		if (_terrainType == TerrainAvailability::ALL || _terrainType == TerrainAvailability::AIR) {
			if (_occupancy == Occupancy::NONE ||
					_occupancy == Occupancy::LAND) {
				return true;
			}
		}
	}
	return false;
	*/
}

bool Tile::isAvailableForPathfinding(Unit::Type unitType) {
	//Check if the unit is moving


	/*
	if (!isAvailable(unitType)) {
		
	}
	*/
	return false;
}

void Tile::setTerrainType(TerrainAvailability terrainType) {
	_terrainType = terrainType;
}

/*
void Tile::setOccupancy(Occupancy occupancy) {
	_occupancy = occupancy;
}
*/

void Tile::setDirection(Direction direction) {
	_direction = direction;
}

void Tile::setLandUnitP(Unit* unit) {
	_landUnitP = unit;
}

void Tile::setAirUnitP(Unit* unit) {
	_airUnitP = unit;
}

void Tile::setNeighbours(std::vector<Tile*> neighbours) {
	this->_neighbours = neighbours;
}

void Tile::setWasVisited(bool wasVisited) {
	this->_wasVisited = wasVisited;
}

void Tile::setG(int G) {
	this->_G = G;
}

void Tile::setH(int H) {
	this->_H = H;
}

void Tile::setParentP(Tile* parentP) {
	this->_parentP = parentP;
}

int Tile::getId() {
	return this->_id;
}

Tile::TerrainAvailability Tile::getTerrainType() {
	return _terrainType;
}

/*
Tile::Occupancy Tile::getOccupancy() {
	return _occupancy;
}
*/

Tile::Direction Tile::getDirection() {
	return _direction;
}

Unit* Tile::getLandUnitP() {
	return _landUnitP;
}

Unit* Tile::getAirUnitP() {
	return _airUnitP;
}

std::vector<Tile*>* Tile::getNeighboursP() {
	return &this->_neighbours;
}

bool Tile::getWasVisited() {
	return this->_wasVisited;
}

int Tile::getG() {
	return this->_G;
}

int Tile::getH() {
	return this->_H;
}

int Tile::getF() {
	return (this->_G + this->_H);
}

Tile* Tile::getParentP() {
	return this->_parentP;
}