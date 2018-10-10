#include "unit.h"
#include "tile.h"
#include "globals.h"
#include "pathfinder.h"
#include "pathparameters.h"
#include "map.h"
#include "engine.h"

#include <iostream>

Unit::Unit() {}

Unit::Unit(Tile* currentTileP, Unit::Type type, Pathfinder* pathfinderP, Map* mapP) :
	_currentTileP(currentTileP),
	_type(type),
	_mapP(mapP)
{
	_engine = new Engine(this, pathfinderP, mapP);
}

Unit::~Unit() {
	delete _engine;
	std::cout << "Destryoing unit!" << std::endl;
}

void Unit::update() {
	//Movement
	_engine->update();
}

Engine* Unit::getEngineP() {
	return _engine;
}

Tile* Unit::getCurrentTileP() {
	return _currentTileP;
}

Unit::Type Unit::getType() {
	return _type;
}

bool Unit::getHovered() {
	return _hovered;
}

bool Unit::getSelected() {
	return _selected;
}

void Unit::setCurrentTileP(Tile* currentTileP) {
	_currentTileP = currentTileP;
}

void Unit::setHovered(bool hovered) {
	_hovered = hovered;
}

void Unit::setSelected(bool selected) {
	_selected = selected;
}
