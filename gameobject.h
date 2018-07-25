#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

class Tile;

/* class GameObject
Parent class for classes Unit and Building
*/
class GameObject {
public:
	GameObject();

	/* GameObject(int id)
	Initializes the object and private variables _column, _row.
	*/
	GameObject(int id);

	~GameObject();		//test purpose only (for now)

	//Setters
	void setTargetTileP(Tile* targetTileP);

	//Getters
	int getId();

private:
	int _id;
};

#endif