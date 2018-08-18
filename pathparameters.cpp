//TODO: This

#include "pathparameters.h"
#include "tile.h"			//is this necessary?
#include "unit.h"

#include <iostream>

PathParameters::PathParameters() {}

PathParameters::PathParameters(Tile* targetP, std::vector<Unit*> units) :
	_targetP(targetP),
	_units(units)		//Not sure whether this line will work
{
	//Test if it works
	//std::cout << "Id of the unit is " << _units[0]->getId() << std::endl;

	//Ok it probably works
}

Tile* PathParameters::getTargetP() {
	return _targetP;
}

std::vector<Unit*>* PathParameters::getUnitsP() {
	return &_units;
}