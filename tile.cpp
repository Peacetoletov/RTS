#include "tile.h"

#include <algorithm>		//std::min, std::max
#include "map.h"		

#include <iostream>

Tile::Tile() {}

Tile::Tile(int id, TerrainAvailability type, Map* mapP) : 
	_id(id),
	_type(type),
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
	this->_wasVisited = false;
	this->_G = INT_MAX;				//Not infinity, but close enough
	this->_H = INT_MAX;				//Not infinity, but close enough
	Tile* _parentP = nullptr;
}

void Tile::setTerrainType(TerrainAvailability type) {
	this->_type = type;
}

void Tile::setOccupancy(Occupancy occupancy) {
	_occupancy = occupancy;
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
	return this->_type;
}

Tile::Occupancy Tile::getOccupancy() {
	return _occupancy;
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