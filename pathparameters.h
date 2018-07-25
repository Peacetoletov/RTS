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
	enum Algorithm {
		A_Star,
		Dijkstra
	};

private:
	Algorithm _algorithm;
	Tile* _targetP;
	std::vector<Unit*> _unitsP;		//Vector of pointers to units
};

#endif