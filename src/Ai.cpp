#include <stdio.h>
#include <math.h>
#include "main.h"

Ai *Ai::create(TCODZip &zip) {
	AiType type = (AiType)zip.getInt();
	Ai *ai = NULL;
	switch(type) {
		case PLAYER:
		    ai = new PlayerAi();
		    break;
		case ENEMY:
		    ai = new EnemyAi();
		    break;
		case CONFUSED_ENEMY:
		    ai = new ConfusedEnemyAi(0, NULL);
		    break;
	}
	ai->load(zip);
	return ai;
}

// how many turns the monster chases the player
// after losing his sight
static const int TRACKING_TURNS = 3;

EnemyAi::EnemyAi() : moveCount(0) {}

void EnemyAi::load(TCODZip &zip) {
	moveCount = zip.getInt();
}

void EnemyAi::save(TCODZip &zip) {
	zip.putInt(ENEMY);
	zip.putInt(moveCount);
}

void EnemyAi::update(Actor *owner) {
    if (owner->destructible && owner->destructible->isDead()) {
    	return;
    }

	if (engine.map->isInFov(owner->x, owner->y)) {
    	// we can see the player moving towards him
    	moveCount = TRACKING_TURNS;
    } else {
    	moveCount--;
    }

   	if (moveCount > 0) {
   		moveOrAttack(owner, engine.player->x, engine.player->y);
   	}
}

void EnemyAi::moveOrAttack(Actor *owner, int targetx, int targety) {
	int dx = targetx - owner->x;
	int dy = targety - owner->y;
	int stepdx = (dx > 0 ? 1 : -1);
	int stepdy = (dy > 0 ? 1 : -1);
	float distance=sqrtf(dx*dx + dy*dy);
	if (distance >= 2) {
		dx = (int)(round(dx / distance));
		dy = (int)(round(dy / distance));
		if (engine.map->canWalk(owner->x+dx, owner->y+dy)) {
			owner->x += dx;
			owner->y += dy;
		} else if (engine.map->canWalk(owner->x+stepdx, owner->y)) {
			owner->x += stepdx;
		} else if (engine.map->canWalk(owner->x, owner->y+stepdy)) {
			owner->y += stepdy;
		}
	} else if (owner->attacker) {
		owner->attacker->attack(owner, engine.player);
	}
}

ConfusedEnemyAi::ConfusedEnemyAi(int nbTurns, Ai *oldAi) : nbTurns(nbTurns), oldAi(oldAi) {}

void ConfusedEnemyAi::load(TCODZip &zip) {
	nbTurns = zip.getInt();
	oldAi = Ai::create(zip);
}

void ConfusedEnemyAi::save(TCODZip &zip) {
	zip.putInt(CONFUSED_ENEMY);
	zip.putInt(nbTurns);
	oldAi->save(zip);
}

void ConfusedEnemyAi::update(Actor *owner) {
	if (!owner->destructible || ! owner->destructible->isDead()) {
		TCODRandom *rng = TCODRandom::getInstance();
		int dx = rng->getInt(-1, 1);
		int dy = rng->getInt(-1, 1);
		if (dx != 0 || dy != 0) {
			int destx = owner->x+dx;
			int desty = owner->y+dy;
			if (engine.map->canWalk(destx, desty)) {
				owner->x = destx;
				owner->y = desty;
			} else {
				Actor *actor = engine.getActor(destx, desty);
				if (actor) {
					owner->attacker->attack(owner, actor);
				}
			}
		}
	}

	nbTurns--;

	if (nbTurns == 0) {
		owner->ai = oldAi;
		delete this;
	}
}

PlayerAi::PlayerAi() : xpLevel(1) {}

void PlayerAi::load(TCODZip &zip) {
	xpLevel = zip.getInt();
}

void PlayerAi::save(TCODZip &zip) {
	zip.putInt(PLAYER);
	zip.putInt(xpLevel);
}

const int LEVEL_UP_BASE = 200;
const int LEVEL_UP_FACTOR = 150;


int PlayerAi::getNextLevelXp() {
	return LEVEL_UP_BASE + xpLevel * LEVEL_UP_FACTOR;
}

void PlayerAi::update(Actor *owner) {
	int levelUpXp = getNextLevelXp();
	if (owner->destructible->xp >= levelUpXp) {
		xpLevel++;
		owner->destructible->xp -= levelUpXp;
		engine.gui->message(TCODColor::yellow, "Your battle skills grow stronger! You reached level %d", xpLevel);
		engine.gui->menu.clear();
		engine.gui->menu.addItem(Menu::HEALTH, "Health (+20HP)");
		engine.gui->menu.addItem(Menu::ATTACK, "Attack (+1 attack)");
		engine.gui->menu.addItem(Menu::DEFENCE, "Defense (+1 defense)");
		engine.gui->menu.addItem(Menu::EVASION, "Evasion (+2 evasion)");
		Menu::MenuItemCode menuItem = engine.gui->menu.pick(Menu::PAUSE);
		switch (menuItem) {
			case Menu::HEALTH :
				owner->destructible->maxHp += 20;
				owner->destructible->hp += 20;
				break;
			case Menu::ATTACK :
				owner->attacker->power += 1;
				break;
			case Menu::DEFENCE :
				owner->destructible->defense += 1;
				break;
            case Menu::EVASION :
				owner->destructible->evasion += 2;
				break;
			default:
			    break;
		}
	}
    if (owner->destructible && owner->destructible->isDead()) {
    	return;
    }
	int dx = 0,dy = 0;
	switch(engine.lastKey.vk) {
		case TCODK_UP:
        case TCODK_KP8:
            dy = -1;
            break;
		case TCODK_DOWN:
        case TCODK_KP2:
            dy = 1;
            break;
		case TCODK_LEFT:
        case TCODK_KP4:
            dx = -1;
            break;
		case TCODK_RIGHT:
        case TCODK_KP6:
            dx = 1;
            break;
		case TCODK_KP7:
		    dy = dx = -1;
		    break;
		case TCODK_KP9:
		    dy = -1;
		    dx = 1;
		    break;
		case TCODK_KP1:
		    dx = -1;
		    dy = 1;
		    break;
		case TCODK_KP3:
		    dx = dy = 1;
		    break;
		case TCODK_KP5:
		    engine.gameStatus = Engine::NEW_TURN;
		    break;
		case TCODK_CHAR:
		    handleActionKey(owner, engine.lastKey.c);
            break;
        default:
            break;
    }
    if (dx != 0 || dy != 0) {
    	engine.gameStatus = Engine::NEW_TURN;
    	if (moveOrAttack(owner, owner->x+dx, owner->y+dy)) {
    		engine.map->computeFov();
    	}
	}
}

bool PlayerAi::moveOrAttack(Actor *owner, int targetx,int targety) {
	if (engine.map->isWall(targetx,targety)) return false;
	// look for living actors to attack
	for (Actor **iterator=engine.actors.begin();
		iterator != engine.actors.end(); iterator++) {
		Actor *actor=*iterator;
		if (actor->destructible && !actor->destructible->isDead() &&
			actor->x == targetx && actor->y == targety) {
			owner->attacker->attack(owner, actor);
			return false;
		}
	}
	// look for body or items
	for (Actor **iterator=engine.actors.begin();
		iterator != engine.actors.end(); iterator++) {
		Actor *actor=*iterator;
		bool corpseOrItem = (actor->destructible && actor->destructible->isDead()) || actor->item;
		if (corpseOrItem && actor->x == targetx && actor->y == targety) {
			engine.gui->message(TCODColor::lightGrey, "There's a %s here.", actor->name);
		}
	}
	owner->x = targetx;
	owner->y = targety;
	return true;
}

void PlayerAi::handleActionKey(Actor *owner, int ascii) {
    switch(ascii) {
        case 'd': // drop item
            handleDropItem(owner);
            break;
        case 'g': // pickup item
            handlePickUpItem(owner);
            break;
        case 'i': // display inventory
            handleInventory(owner);
            break;
        case '/': // handle stairs
            if (engine.stairs->x == owner->x && engine.stairs->y == owner->y) {
				engine.nextLevel();
			} else {
				engine.gui->message(TCODColor::lightGrey, "There are no stairs here.");
			}
            break;
        default:
            break;
    }
}

void PlayerAi::handleDropItem(Actor *owner) {
    Actor *actor = choseFromInventory(owner);

    if (actor) {
        actor->item->drop(actor, owner);
        engine.gameStatus = Engine::NEW_TURN;
    }
}

void PlayerAi::handlePickUpItem(Actor *owner) {
    bool found = false;

    for (Actor **iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
        Actor *actor = *iterator;

        if (actor->item && actor->x == owner->x && actor->y == owner->y) {
            if (actor->item->pick(actor, owner)) {
                found = true;
                engine.gui->message(TCODColor::lightGrey, "You pick the %s.",
                                   actor->name);
                break;
            } else if (!found) {
                found = true;
                engine.gui->message(TCODColor::red, "Your inventory is full.");
            }
        }
    }

    if (!found) {
        engine.gui->message(TCODColor::lightGrey, "There's nothing here that you can pick.");
    }

    engine.gameStatus = Engine::NEW_TURN;
}

void PlayerAi::handleInventory(Actor *owner) {
    Actor *actor = choseFromInventory(owner);
    if (actor) {
        actor->item->use(actor,owner);
        engine.gameStatus = Engine::NEW_TURN;
    }
}

Actor *PlayerAi::choseFromInventory(Actor *owner) {
	static const int INVENTORY_WIDTH = 50;
	static const int INVENTORY_HEIGHT = 28;
	static TCODConsole con(INVENTORY_WIDTH, INVENTORY_HEIGHT);

	// display the inventory frame
	con.setDefaultForeground(TCODColor(200, 180, 50));
	con.printFrame(0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, "inventory");

	// display the items with their keyboard shortcut
	con.setDefaultForeground(TCODColor::white);
	int shortcut = 'a';
	int y = 1;
	for (Actor **it = owner->container->inventory.begin();
		it != owner->container->inventory.end(); it++) {
		Actor *actor = *it;
		con.print(2, y, "(%c) %s", shortcut, actor->name);
		y++;
		shortcut++;
	}

	// blit the inventory console on the root console
	TCODConsole::blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT,
		TCODConsole::root, engine.screenWidth/2 - INVENTORY_WIDTH/2,
		engine.screenHeight/2 - INVENTORY_HEIGHT/2);
	TCODConsole::flush();

	// wait for a key press
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
	if (key.vk == TCODK_CHAR) {
		int actorIndex = key.c - 'a';
		if (actorIndex >= 0 && actorIndex < owner->container->inventory.size()) {
			return owner->container->inventory.get(actorIndex);
		}
	}
	return NULL;
}
