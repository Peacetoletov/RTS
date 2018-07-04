#include "tile.h"

Tile::Tile() {}

Tile::Tile(int id, TerrainAvailability type) : 
	_id(id),
	_type(type)
{

}

void Tile::setNeighbours(Tile** neighbours) {
	this->_neighbours = neighbours;
}