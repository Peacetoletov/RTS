#include "gameobject.h"
#include "tile.h"

#include <iostream>

GameObject::GameObject() {}

GameObject::GameObject(int id) :
	_id(id)
{

}

GameObject::GameObject(int id, Tile* targetTileP) :
	_id(id),
	_targetTileP(targetTileP)
{

}

GameObject::~GameObject() {
	//std::cout << "Destroying game object!";
}

void GameObject::setTargetTileP(Tile* targetTileP) {
	//std::cout << "Target tile id = " << targetTileP->getId() << std::endl;
	this->_targetTileP = targetTileP;
}

int GameObject::getId() {
	return _id;
}

Tile* GameObject::getTargetTileP() {
	return this->_targetTileP;
}