#ifndef TILE_H
#define TILE_H

class Tile {
public:
	Tile();			//Won't be used

	/* Tile
	Creates the object and initializes _row, _column
	*/
	Tile(int id);

	/* void setNeighbours
	Sets **_neighbours
	*/
	void setNeighbours(Tile** neighbours);

private:
	int _id;
	Tile** _neighbours;		//Pointer to an array holding pointers to neighbour tiles
	//TODO: Check if the array really has to be holding pointers, if plain objects 
	//in the array wouldn't be sufficient (Tile* instead of Tile**)
	//nvm, I checked it and looks like double pointer is necessary
};

#endif