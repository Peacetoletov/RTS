#ifndef UNIT_H
#define UNIT_H

class Unit {
public:
	Unit();		//Won't be used

	Unit(int id);

	~Unit();		//Not used yet

	//Getters
	int getId();

	//Setters
	void setId(int id);

private:
	int _id;
};

#endif