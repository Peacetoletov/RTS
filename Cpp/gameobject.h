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
	GameObject(int column, int row);

	~GameObject();		//test purpose only (for now)

	/* int getColumn
	Returns _column.
	*/
	int getColumn();

	/* int getRow
	Returns _row.
	*/
	int getRow();

private:
	int _column;
	int _row;
};

#endif