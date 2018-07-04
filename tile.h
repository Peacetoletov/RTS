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
	Creates the object and initializes _id, _type.
	*/
	Tile(int id, TerrainAvailability type);

	/* ~Tile
	Test purpose only (for now)
	*/
	~Tile();

	/* void setNeighbours
	Sets **_neighbours.
	*/
	void setNeighbours(Tile** neighbours);

	/* void setType
	Sets _type.
	*/
	void setType(TerrainAvailability type);

	//Getters
	TerrainAvailability getType();

private:
	int _id;
	TerrainAvailability _type;

	Tile** _neighbours;		//Pointer to an array holding pointers to neighbour tiles
};

#endif