#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

/* class GameObject
Parent class for classes Unit and Building
*/
class GameObject {
public:
	GameObject();

	/* GameObject(int column, int row)
	Initializes the object and private variables _column, _row.
	*/
	GameObject(int id);

	~GameObject();		//test purpose only (for now)

	int getId();

private:
	int _id;
};

#endif