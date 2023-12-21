#ifndef _ITEM_H_
#define _ITEM_H_

class Item : public Object {
public :
	bool pick(Actor *owner, Actor *wearer);
	void drop(Actor *owner, Actor *wearer);
	virtual bool use(Actor *owner, Actor *wearer);
	static Item *create (TCODZip &zip);
protected :
	enum ItemType {
		HEALER,SNAP_SHOT,CONFUSE_SHOT,WIDE_SHOT
	};
};

class Healer : public Item {
public :
	float amount; // how many hp

	Healer(float amount);
	bool use(Actor *owner, Actor *wearer);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};

class SnapShot : public Item {
public :
	float range, damage;
	SnapShot(float range, float damage);
	bool use(Actor *owner, Actor *wearer);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};

class ConfuseShot : public Item {
public :
	int nbTurns;
	float range;
	ConfuseShot(int nbTurns, float range);
	bool use(Actor *owner, Actor *wearer);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};

class WideShot : public SnapShot {
public :
	WideShot(float range, float damage);
	bool use(Actor *owner, Actor *wearer);
	void save(TCODZip &zip);
};

#endif // _ITEM_H_

