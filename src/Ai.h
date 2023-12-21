#ifndef _AI_H_
#define _AI_H_

class Ai : public Object {
public :
	virtual void update(Actor *owner) = 0;
	static Ai *create(TCODZip &zip);
protected :
	enum AiType {
		ENEMY, CONFUSED_ENEMY, PLAYER
	};
};

class EnemyAi : public Ai {
public :
	EnemyAi();
	void update(Actor *owner);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
protected :
	int moveCount;

	void moveOrAttack(Actor *owner, int targetx, int targety);
};

class ConfusedEnemyAi : public Ai {
public :
	ConfusedEnemyAi(int nbTurns, Ai *oldAi);
	void update(Actor *owner);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
protected :
	int nbTurns;
	Ai *oldAi;
};

class PlayerAi : public Ai {
public :
	int xpLevel;
	PlayerAi();
	int getNextLevelXp();
	void update(Actor *owner);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
protected :
	bool moveOrAttack(Actor *owner, int targetx, int targety);
	void handleActionKey(Actor *owner, int ascii);
	Actor *choseFromInventory(Actor *owner);
private:
    void handleDropItem(Actor *owner);
    void handlePickUpItem(Actor *owner);
    void handleInventory(Actor *owner);
};
#endif // _AI_H_

