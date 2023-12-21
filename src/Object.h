#ifndef _OBJECT_H_
#define _OBJECT_H_

class Object {
public :
	virtual void load(TCODZip &zip) = 0;
	virtual void save(TCODZip &zip) = 0;
};
#endif // _OBJECT_H_
