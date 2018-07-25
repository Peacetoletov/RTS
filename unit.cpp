//TODO: This

#include "unit.h"

Unit::Unit() {}

Unit::Unit(int id) :
	_id(id)
{

}

Unit::~Unit() {

}

int Unit::getId() {
	return _id;
}

void Unit::setId(int id) {
	_id = id;
}