#ifndef MAP_H
#define MAP_H

#include <vector>
#include "gameobject.h"
#include "tile.h"

using namespace std;

/* class Map
This class contains information about the tiles on the map.
*/

class Map {
public:

	Map();			//Won't be used

	/* Map
	Initializes terrain with all tiles available to all units.
	*/
	Map(int rows, int columns);

	/* ~Map
	Deletes instances of GameObject in _objects
	*/
	~Map();

	/* void loadTestMap
	Changes values of _terrain to new values defined in this function.
	*/
	void loadTestMap();

	/* void loadTestObjects 
	Creates instances (defined in this function) of GameObject in _objects to test the pathfinder
	*/
	void loadTestObjects();

	/* int idToRow
	Returns the row based on the id
	*/
	int idToRow(int id);

	/* int idToColumn
	Returns the column based on the id
	*/
	int idToColumn(int id);

	/* int positionToId
	Returns the id based on the row and column
	*/
	int positionToId(int row, int column);

	//Getters
	int getRows();
	int getColumns();
	vector<GameObject*>* getObjectsP();
	Tile** getTilesP();

private:
	//VARIABLES
	int _rows;
	int _columns;

	/* _objects
	Contains information about units and building on the map.
	*/
	vector<GameObject*> _objects;		//Alternatively, this can be divided into 2 separate vectors,
										//1 containing  units and the other one buildings.
	/* Tile** _tiles
	Pointer to an array of pointers to instances of Tile object.
	_tiles[i] points to a tile with id i.
	Example: _tiles[20] points to a tile with id 20.
	*/
	Tile** _tiles;						

	//METHODS
	/* void setNeighbours(Tile* tile, Tile** tiles)
	Sets neighbours of tiles[id] (id = arg1) as pointers to other tiles from array tiles (arg2)
	*/
	void setNeighbours(int id, Tile** tiles);

};

#endif