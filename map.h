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
	/* enum TerrainAvailability
	Indicates which units can move to a tile of that type.
	*/
	enum TerrainAvailability {
		ALL,
		AIR,
		NONE
	};

	Map();

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

	/* vector<vector<TerrainAvailability> >* getTerrainP
	Returns a pointer to _terrain.
	*/
	vector<vector<TerrainAvailability> >* getTerrainP();

	/* vector<GameObject*>* getObjectsP
	Returns a pointer to _objects.
	*/
	vector<GameObject*>* getObjectsP();

private:
	/* void setTerrainTile
	Sets a tile in the map to a certain type, based on the terrain availability.
	*/
	void setTerrainTile(int row, int column, TerrainAvailability type);

	/* _terrain
	Contains information about each tile and its availability to different types of units.
	*/
	vector<vector<TerrainAvailability> > _terrain;		//TODO: Remove this		//also, if I wanted to do it this way, I should use a 2d dynamically allocated array, not a vector

	/* _objects
	Contains information about units and building on the map.
	*/
	vector<GameObject*> _objects;		//Alternatively, this can be divided into 2 separate vectors,
										//1 containing  units and the other one buildings.

	Tile** _tiles;						//Pointer to an array of pointers to instances of Tile object

};

#endif