#include "tile.h"

#include <iostream>

Tile::Tile() {}

Tile::Tile(int id, TerrainAvailability type) : 
	_id(id),
	_type(type)
{

}

Tile::~Tile() {
	//std::cout << "Deleting tile " << this->_id << std::endl;
}

void Tile::setNeighbours(Tile** neighbours) {
	this->_neighbours = neighbours;
}

void Tile::setType(TerrainAvailability type) {
	this->_type = type;
}

Tile::TerrainAvailability Tile::getType() {
	return this->_type;
}