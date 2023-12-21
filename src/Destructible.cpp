#include <stdio.h>
#include "main.h"

Destructible::Destructible(float maxHp, float defense, const char *corpseName, int xp) :
	maxHp(maxHp), hp(maxHp), defense(defense), corpseName(corpseName), xp(xp) {
}

Destructible *Destructible::create(TCODZip &zip) {
	DestructibleType type = (DestructibleType)zip.getInt();
	Destructible *destructible = NULL;
	switch(type) {
		case ENEMY:
		    destructible = new EnemyDestructible(0, 0, NULL, 0);
		    break;
		case PLAYER:
		    destructible = new PlayerDestructible(0, 0, NULL);
		    break;
	}
	destructible->load(zip);
	return destructible;
}

void Destructible::load(TCODZip &zip) {
	maxHp = zip.getFloat();
	hp = zip.getFloat();
	defense = zip.getFloat();
	evasion = zip.getFloat();
	corpseName = strdup(zip.getString());
	xp = zip.getInt();
}

void Destructible::save(TCODZip &zip) {
	zip.putFloat(maxHp);
	zip.putFloat(hp);
	zip.putFloat(defense);
	zip.putFloat(evasion);
	zip.putString(corpseName);
	zip.putInt(xp);
}

float Destructible::takeDamage(Actor *owner, float damage) {
	if (damage <= 0) {
        damage = 0;
    } else {
        damage -= defense;
        if (damage > 0) {
            hp -= damage;
            if (hp <= 0) {
                die(owner);
            }
        }
    }
	return damage;
}

float Destructible::heal(float amount) {
	hp += amount;
	if (hp > maxHp) {
		amount -= hp-maxHp;
		hp = maxHp;
	}
	return amount;
}

void Destructible::die(Actor *owner) {
    transformIntoCorpse(owner);
    // genrate corpses are drawn before living actors
    engine.sendToBack(owner);
}

void Destructible::transformIntoCorpse(Actor *owner) {
    // transform the actor into a corpse
    owner->ch = '%';
    owner->col = TCODColor::darkRed;
    owner->name = corpseName;
    owner->blocks = false;
}

EnemyDestructible::EnemyDestructible(float maxHp, float defense, const char *corpseName, int xp) :
	Destructible(maxHp, defense, corpseName, xp) {
}

void EnemyDestructible::save(TCODZip &zip) {
	zip.putInt(ENEMY);
	Destructible::save(zip);
}

void EnemyDestructible::die(Actor *owner) {
	// transform it into corpse
	// attacked and doesn't move
	engine.gui->message(TCODColor::lightGrey, "%s is dead. You gain %d xp", owner->name, xp);
	engine.player->destructible->xp += xp;
	Destructible::die(owner);
}

PlayerDestructible::PlayerDestructible(float maxHp, float defense, const char *corpseName) :
	Destructible(maxHp, defense, corpseName, 0) {
}

void PlayerDestructible::die(Actor *owner) {
	engine.gui->message(TCODColor::red, "You died!");
	Destructible::die(owner);
	engine.gameStatus = Engine::DEFEAT;
}

void PlayerDestructible::save(TCODZip &zip) {
	zip.putInt(PLAYER);
	Destructible::save(zip);
}
