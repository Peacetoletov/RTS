//TODO: This

#include "pathparameters.h"
#include "tile.h"			//is this necessary?
#include "unit.h"

#include <iostream>

PathParameters::PathParameters() {}

PathParameters::PathParameters(Algorithm algorithm, Tile* targetP, std::vector<Unit*>* unitsP) :
	_algorithm(algorithm),
	_targetP(targetP),
	_unitsP(unitsP)		//Not sure whether this line will work
{
	//Test if it works
	//std::cout << "Id of the unit is " << _units[0]->getId() << std::endl;

	//Ok it probably works
}

PathParameters::Algorithm PathParameters::getAlgorithm() {
	return _algorithm;
}

Tile* PathParameters::getTargetP() {
	return _targetP;
}

std::vector<Unit*>* PathParameters::getUnitsP() {
	return _unitsP;
}