#include "gameobject.h"
#include <iostream>

GameObject::GameObject() {}

GameObject::GameObject(int column, int row) : 
	_column(column),
	_row(row)
{

}

GameObject::~GameObject() {
	//std::cout << "Destroying game object!";
}

int GameObject::getColumn() {
	return _column;
}

int GameObject::getRow() {
	return _row;
}