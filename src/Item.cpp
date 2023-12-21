#include "main.h"

bool Item::pick(Actor *owner, Actor *wearer) {
	if (wearer->container && wearer->container->add(owner)) {
		engine.actors.remove(owner);
		return true;
	}
	return false;
}

void Item::drop(Actor *owner, Actor *wearer) {
	if (wearer->container) {
		wearer->container->remove(owner);
		engine.actors.push(owner);
		owner->x = wearer->x;
		owner->y = wearer->y;
		engine.gui->message(TCODColor::lightGrey, "%s drops a %s.", wearer->name, owner->name);
	}
}

bool Item::use(Actor *owner, Actor *wearer) {
	if (wearer->container) {
		wearer->container->remove(owner);
		delete owner;
		return true;
	}
	return false;
}

Healer::Healer(float amount) : amount(amount) {}

bool Healer::use(Actor *owner, Actor *wearer) {
	if (wearer->destructible) {
		float amountHealed = wearer->destructible->heal(amount);
		if (amountHealed > 0) {
            engine.gui->message(TCODColor::lightGreen, "You feeled refreshed.");
			return Item::use(owner, wearer);
		}
	}
	return false;
}

void Healer::load(TCODZip &zip) {
	amount = zip.getFloat();
}

void Healer::save(TCODZip &zip) {
	zip.putInt(HEALER);
	zip.putFloat(amount);
}

SnapShot::SnapShot(float range, float damage) : range(range), damage(damage) {}

bool SnapShot::use(Actor *owner, Actor *wearer) {
	Actor *closestEnemy=engine.getClosestEnemy(wearer->x, wearer->y, range);
	if (!closestEnemy) {
		engine.gui->message(TCODColor::lightGrey, "No enemy is close enough to strike.");
		return false;
	}
	// hit closest monster for <damage> hit points
	engine.gui->message(TCODColor::lightBlue,
		"The player fires a sudden shot to the %s!\n"
		"The damage is %g hit points.",
		closestEnemy->name, damage);
	closestEnemy->destructible->takeDamage(closestEnemy, damage);
	return Item::use(owner, wearer);
}

void SnapShot::load(TCODZip &zip) {
	range = zip.getFloat();
	damage = zip.getFloat();
}

void SnapShot::save(TCODZip &zip) {
	zip.putInt(SNAP_SHOT);
	zip.putFloat(range);
	zip.putFloat(damage);
}

ConfuseShot::ConfuseShot(int nbTurns, float range) : nbTurns(nbTurns), range(range) {}

bool ConfuseShot::use(Actor *owner, Actor *wearer) {
	Actor *closestEnemy=engine.getClosestEnemy(wearer->x, wearer->y, range);
	if (!closestEnemy) {
		engine.gui->message(TCODColor::lightGrey, "No enemy is close enough to confuse.");
		return false;
	}
	// confuse the monster for <nbTurns> turns
	Ai *confusedAi = new ConfusedEnemyAi(nbTurns, closestEnemy->ai);
	closestEnemy->ai = confusedAi;
	engine.gui->message(TCODColor::lightGreen, "The eyes of the %s look vacant,\nas he/she starts to stumble around!", closestEnemy->name);
	return Item::use(owner, wearer);
}

void ConfuseShot::load(TCODZip &zip) {
	nbTurns = zip.getInt();
	range = zip.getFloat();
}

void ConfuseShot::save(TCODZip &zip) {
	zip.putInt(CONFUSE_SHOT);
	zip.putInt(nbTurns);
	zip.putFloat(range);
}

WideShot::WideShot(float range, float damage) : SnapShot(range, damage) {}

bool WideShot::use(Actor *owner, Actor *wearer) {
	Actor *closestEnemy=engine.getClosestEnemy(wearer->x, wearer->y, range);
	if (!closestEnemy) {
		engine.gui->message(TCODColor::lightGrey, "No enemy is close enough to strike.");
		return false;
	}
	int x = wearer->x, y = wearer->y;
	// attack everything in <range> (including player)
	engine.gui->message(TCODColor::orange, "The player fires a wide shot, destroying anything within %g tiles!", range);
	for (Actor **iterator = engine.actors.begin();
	    iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor->destructible && !actor->destructible->isDead() && actor->getDistance(x, y) <= range) {
			engine.gui->message(TCODColor::orange, "The damage %s got is %g hit points.", actor->name, damage);
			actor->destructible->takeDamage(actor, damage);
		}
	}
	return Item::use(owner, wearer);
}

void WideShot::save(TCODZip &zip) {
	zip.putInt(WIDE_SHOT);
	zip.putFloat(range);
	zip.putFloat(damage);
}

Item *Item::create(TCODZip &zip) {
	ItemType type = (ItemType)zip.getInt();
	Item *item = NULL;
	switch(type) {
		case HEALER : item = new Healer(0); break;
		case SNAP_SHOT : item = new SnapShot(0,0); break;
		case CONFUSE_SHOT : item = new ConfuseShot(0,0); break;
		case WIDE_SHOT : item = new WideShot(0,0); break;
	}
	item->load(zip);
	return item;
}
