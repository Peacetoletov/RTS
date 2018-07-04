#include "tile.h"

Tile::Tile() {}

Tile::Tile(int id) : 
	_id(id)
{

}

void Tile::setNeighbours(Tile** neighbours) {
	this->_neighbours = neighbours;
}