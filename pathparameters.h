#ifndef PATH_PARAMETERS_H
#define PATH_PARAMETERS_H

#include <vector>

/* class PathParameters
This class specifies parameters of a path that I want to find. It includes the type of algorithm
(A* for 1 unit, Dijkstra for a group of units), the target of the path and all selected units.
*/

class Tile;
class Unit;

class PathParameters {
public:
	PathParameters();		//Won't be used

	PathParameters(Tile* _targetP, std::vector<Unit*> _units, int groupId);

	//Getters
	Tile* getTargetP();
	std::vector<Unit*>* getUnitsP();
	int getGroupId();

private:
	Tile* _targetP;
	std::vector<Unit*> _units;		//Vector of pointers to units in the unit group
	int _groupId;
};

#endif