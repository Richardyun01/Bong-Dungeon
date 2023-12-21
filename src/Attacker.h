#ifndef _ATTACKER_H_
#define _ATTACKER_H_

class Attacker : public Object {
public :
	float power;    // hit points given
	float evasion;  // evasion points given

	Attacker(float power, float evasion);
	void attack(Actor *owner, Actor *target);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};
#endif // _ATTACKER_H_
