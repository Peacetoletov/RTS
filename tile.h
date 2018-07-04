#ifndef TILE_H
#define TILE_H

class Tile {
public:
	enum TerrainAvailability {
		ALL,
		AIR,
		NONE
	};

	Tile();			//Won't be used

	/* Tile
	Creates the object and initializes _id, _type
	*/
	Tile(int id, TerrainAvailability type);

	/* void setNeighbours
	Sets **_neighbours
	*/
	void setNeighbours(Tile** neighbours);

private:
	int _id;
	TerrainAvailability _type;

	Tile** _neighbours;		//Pointer to an array holding pointers to neighbour tiles
};

#endif