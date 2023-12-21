#include <stdio.h>
#include <cstdlib>
#include "main.h"

Attacker::Attacker(float power, float evasion) : power(power), evasion(evasion) {}

void Attacker::attack(Actor *owner, Actor *target) {
	if (target->destructible && ! target->destructible->isDead()) {
        // Generate a random value between 0 and 99 for evasion check
		int evasionRoll = rand() % 100;

		if (evasionRoll < evasion) {
			// Evade the attack
			engine.gui->message(TCODColor::lightGrey, "%s evades the attack from %s!", owner->name, target->name);
		} else {
            if (power - target->destructible->defense > 0) {
                engine.gui->message(owner == engine.player ? TCODColor::red : TCODColor::lightGrey,
                    "%s attacks %s for %g hit points.", owner->name, target->name,
                    power - target->destructible->defense);
            } else {
                engine.gui->message(TCODColor::lightGrey, "%s attacks %s but it has no effect!", owner->name, target->name);
            }
            target->destructible->takeDamage(target, power);
		}
	} else {
		engine.gui->message(TCODColor::lightGrey, "%s attacks %s in vain.", owner->name, target->name);
	}
}

void Attacker::load(TCODZip &zip) {
	power = zip.getFloat();
}

void Attacker::save(TCODZip &zip) {
	zip.putFloat(power);
}
