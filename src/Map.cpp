#include <cmath>
#include "main.h"

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;
static const int MAX_ROOM_ENEMIES = 3;
static const int MAX_ROOM_ITEMS = 2;

class BspListener : public ITCODBspCallback {
private :
    Map &map;           // a map to dig
    int roomNum;        // room number
    int lastx, lasty;   // center of the last room
public :
    BspListener(Map &map) : map(map), roomNum(0) {}
    bool visitNode(TCODBsp *node, void *userData) {
    	if (node->isLeaf()) {
    		int x, y, w, h;
			// dig a room
			bool withActors = (bool)userData;
			w = map.rng->getInt(ROOM_MIN_SIZE, node->w-2);
			h = map.rng->getInt(ROOM_MIN_SIZE, node->h-2);
			x = map.rng->getInt(node->x+1, node->x+node->w-w-1);
			y = map.rng->getInt(node->y+1, node->y+node->h-h-1);
			map.createRoom(roomNum == 0, x, y, x+w-1, y+h-1, withActors);
			if (roomNum != 0) {
			    // dig a corridor from last room
			    map.dig(lastx, lasty, x+w/2, lasty);
			    map.dig(x+w/2, lasty, x+w/2, y+h/2);
			}
            lastx = x+w/2;
            lasty = y+h/2;
            roomNum++;
        }
        return true;
    }
};

Map::Map(int width, int height) : width(width), height(height) {
	seed = TCODRandom::getInstance()->getInt(0, 0x7FFFFFFF);
}

void Map::init(bool withActors) {
	rng = new TCODRandom(seed, TCOD_RNG_CMWC);
    tiles = new Tile[width * height];
    map = new TCODMap(width, height);
    TCODBsp bsp(0, 0, width, height);
    bsp.splitRecursive(rng, 8, ROOM_MAX_SIZE,ROOM_MAX_SIZE, 1.5f, 1.5f);
    BspListener listener(*this);
    bsp.traverseInvertedLevelOrder(&listener, (void *)withActors);
}

Map::~Map() {
    delete [] tiles;
    delete map;
}

void Map::load(TCODZip &zip) {
	seed = zip.getInt();
    init(false);
	for (int i = 0; i < width * height; i++) {
		tiles[i].explored = zip.getInt();
	}
}

void Map::save(TCODZip &zip) {
	zip.putInt(seed);
	for (int i = 0; i < width * height; i++) {
		zip.putInt(tiles[i].explored);
	}
}

void Map::dig(int x1, int y1, int x2, int y2) {
    if (x2 < x1) {
        int tmp = x2;
        x2 = x1;
        x1 = tmp;
    }
    if (y2 < y1) {
        int tmp = y2;
        y2 = y1;
        y1 = tmp;
    }
    for (int tilex=x1; tilex <= x2; tilex++) {
        for (int tiley=y1; tiley <= y2; tiley++) {
            map->setProperties(tilex, tiley, true, true);
        }
    }
}

void Map::addEnemy(int x, int y) {
	TCODRandom *rng = TCODRandom::getInstance();
	int mobGenerator = rng->getInt(0, 200);  // random generator
	int levelScaler = engine.getLevel();     // level scaler
	if (levelScaler >= 50) levelScaler = 50; // maximum limit of level scale
    if (mobGenerator < 160) {
        // create an oop
        Actor *oop = new Actor(x, y, 'o', "oop", TCODColor::desaturatedGreen);
        oop->destructible = new EnemyDestructible(10+floor(levelScaler/3), 0+floor(levelScaler/6), "dead oop", 35);
        oop->attacker = new Attacker(3+floor(levelScaler/6), 0+floor(levelScaler/6));
        oop->ai = new EnemyAi();
        engine.actors.push(oop);
    } else if (mobGenerator >= 160 && mobGenerator <= 195) {
        // create a c++
        Actor *cplus = new Actor(x, y, 'C', "c++", TCODColor::darkerGreen);
        cplus->destructible = new EnemyDestructible(16+floor(levelScaler/3), 1+floor(levelScaler/5), "c++ remains", 100);
        cplus->attacker = new Attacker(4+floor(levelScaler/5), 1+floor(levelScaler/5));
        cplus->ai = new EnemyAi();
        engine.actors.push(cplus);
    } else {
        if (levelScaler >= 5 && mobGenerator >= 198) {
            // create a bong
            Actor *cplus = new Actor(x, y, 'B', "BONG", TCODColor::darkerGreen);
            cplus->destructible = new EnemyDestructible(230+floor(levelScaler-4), 3+floor(levelScaler-4), "no BONG", 500);
            cplus->attacker = new Attacker(7+floor(levelScaler-4), 3+floor(levelScaler-4));
            cplus->ai = new EnemyAi();
            engine.actors.push(cplus);
        } else {
            // create a test
            Actor *cplus = new Actor(x, y, 'T', "test", TCODColor::darkerGreen);
            cplus->destructible = new EnemyDestructible(20+floor(levelScaler/3), 1+floor(levelScaler/4), "destroyed test", 150);
            cplus->attacker = new Attacker(5+floor(levelScaler/3), 2+floor(levelScaler/3));
            cplus->ai = new EnemyAi();
            engine.actors.push(cplus);
        }
    }
}

void Map::addItem(int x, int y) {
	TCODRandom *rng = TCODRandom::getInstance();
	int dice = rng->getInt(0, 200);
	if (dice < 140) {
		// create a health potion
		Actor *healthPotion = new Actor(x, y, '!', "health potion", TCODColor::violet);
		healthPotion->blocks = false;
		healthPotion->item = new Healer(4);
		engine.actors.push(healthPotion);
	} else if (140 <= dice && dice < 140+20) {
		// create a snap shot
		Actor *snapShot = new Actor(x, y, '#', "snap shot", TCODColor::lightYellow);
		snapShot->blocks = false;
		snapShot->item = new SnapShot(5, 20);
		engine.actors.push(snapShot);
	} else if (140+20 <= dice && dice < 140+20+10) {
		// create a wide shot
		Actor *wideShot = new Actor(x, y, '#', "wide shot", TCODColor::lightYellow);
		wideShot->blocks = false;
		wideShot->item = new WideShot(7, 12);
		engine.actors.push(wideShot);
	} else if (140+20+10 <= dice && dice < 140+20+10+10) {
		// create a confuse shot
		Actor *confuseShot = new Actor(x, y, '#', "confuse shot", TCODColor::lightYellow);
		confuseShot->blocks = false;
		confuseShot->item = new ConfuseShot(10, 8);
		engine.actors.push(confuseShot);
	} else {
        // create a super health potion
		Actor *superhealthPotion = new Actor(x, y, '?', "super health potion", TCODColor::violet);
		superhealthPotion->blocks = false;
		superhealthPotion->item = new Healer(20);
		engine.actors.push(superhealthPotion);
	}
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2, bool withActors) {
    dig (x1, y1, x2, y2);
    if (!withActors) {
    	return;
    }
    if (first) {
        // put the player in the first room
        engine.player->x = (x1 + x2) / 2;
        engine.player->y = (y1 + y2) / 2;
    } else {
		TCODRandom *rng = TCODRandom::getInstance();
		// add monsters
		int nbEnemies = rng->getInt(0, MAX_ROOM_ENEMIES);
		while (nbEnemies > 0) {
		    int x = rng->getInt(x1, x2);
		    int y = rng->getInt(y1, y2);
    		if (canWalk(x, y)) {
				addEnemy(x, y);
			}
		    nbEnemies--;
		}
		// add items
		int nbItems = rng->getInt(0, MAX_ROOM_ITEMS);
		while (nbItems > 0) {
		    int x = rng->getInt(x1, x2);
		    int y = rng->getInt(y1, y2);
    		if (canWalk(x, y)) {
				addItem(x, y);
			}
		    nbItems--;
		}
		// set stairs position
		engine.stairs->x = (x1 + x2) / 2;
		engine.stairs->y = (y1 + y2) / 2;
    }
}

bool Map::isWall(int x, int y) const {
    return !map->isWalkable(x, y);
}

bool Map::canWalk(int x, int y) const {
    if (isWall(x, y)) {
        // this is a wall
        return false;
    }
    for (Actor **iterator = engine.actors.begin();
        iterator != engine.actors.end(); iterator++) {
        Actor *actor = *iterator;
        if (actor->blocks && actor->x == x && actor->y == y) {
            // there is a blocking actor here. cannot walk
            return false;
        }
    }
    return true;
}

bool Map::isExplored(int x, int y) const {
    return tiles[x + y * width].explored;
}

bool Map::isInFov(int x, int y) const {
	if (x < 0 || x >= width || y < 0 || y >= height) {
		return false;
	}
    if (map->isInFov(x, y)) {
        tiles[x + y * width].explored=true;
        return true;
    }
    return false;
}

void Map::computeFov() {
    map->computeFov(engine.player->x, engine.player->y, engine.fovRadius);
}

void Map::render() const {
    static const TCODColor darkWall(0, 0, 100);
    static const TCODColor darkGround(50, 50, 150);
	static const TCODColor lightWall(130, 110, 50);
	static const TCODColor lightGround(200, 180, 50);

	for (int x = 0; x < width; x++) {
	    for (int y = 0; y < height; y++) {
	        if (isInFov(x, y)) {
	            TCODConsole::root->setCharBackground(x, y, isWall(x, y) ? lightWall : lightGround);
	        } else if (isExplored(x, y)) {
	            TCODConsole::root->setCharBackground(x, y, isWall(x, y) ? darkWall : darkGround);
	        }
   	    }
	}
}
