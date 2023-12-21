#ifndef _CONTAINER_H_
#define _CONTAINER_H_

class Container : public Object {
public :
	int size; // maximum number of actors. 0=unlimited
	TCODList<Actor *> inventory;

	Container(int size);
	~Container();
	bool add(Actor *actor);
	void remove(Actor *actor);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};
#endif // _CONTAINER_H_

