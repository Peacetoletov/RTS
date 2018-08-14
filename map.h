#ifndef MAP_H
#define MAP_H

#include <vector>
#include "unit.h"			//This is necessary because of Unit::Type enum

class Pathfinder;
class Tile;

/* class Map
This class contains information about the tiles on the map.
*/

class Map {
public:

	Map();			//Won't be used

	/* Map
	Initializes terrain with all tiles available to all units.
	*/
	Map(int rows, int columns, Pathfinder* pathfinderP);

	/* ~Map
	Deletes instances of GameObject in _objects
	*/
	~Map();

	/* void loadTestMap
	Changes values of _terrain to new values defined in this function.
	*/
	void loadTestMap(); 

	/* void loadTestObjects 
	Creates units to test pathfinder and other functionalities
	*/
	void loadTestUnits(int amount, int row[], int column[], Unit::Type type[]);

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

	void update();

	//Setters
	void setSelectedUnits(std::vector<Unit*> selectedUnits);

	//Getters
	int getRows();
	int getColumns();
	std::vector<Unit*>* getUnitsP();
	std::vector<Unit*>* getSelectedUnitsP();
	Tile** getTilesP();

private:
	//VARIABLES
	int _rows;
	int _columns;
	Pathfinder* _pathfinderP;

	/*
	//_objects: Contains information about units and building on the map.
	std::vector<GameObject*> _objects;		//Alternatively, this can be divided into 2 separate vectors,
										//1 containing  units and the other one buildings.
	*/
	std::vector<Unit*> _units;				
	//std::vector<Building*> _buildings;

	std::vector<Unit*> _selectedUnits;

	/* Pointer to an array of pointers to instances of Tile object.
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