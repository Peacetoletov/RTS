#include "gameobject.h"
#include <iostream>

GameObject::GameObject() {}

GameObject::GameObject(int id) : 
	_id(id)
{

}

GameObject::~GameObject() {
	//std::cout << "Destroying game object!";
}

int GameObject::getId() {
	return _id;
}