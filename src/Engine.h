#ifndef _ENGINE_H_
#define _ENGINE_H_

class Engine {
public :
	enum GameStatus {
		STARTUP,
		IDLE,
		NEW_TURN,
		VICTORY,
		DEFEAT
	} gameStatus;
	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;
    TCODList<Actor *> actors;
    Actor *player;
    Actor *stairs;
    Map *map;
    int fovRadius;
    int screenWidth;
    int screenHeight;
    Gui *gui;
    int level;

    Engine(int screenWidth, int screenHeight);
    ~Engine();
    int getLevel() const;
    void update();
    void render();
    void sendToBack(Actor *actor);
    Actor *getActor(int x, int y) const;
    Actor *getClosestEnemy(int x, int y, float range) const;
    bool tileSelection(int *x, int *y, float maxRange = 0.0f);
    void nextLevel();
    void initializeNewLevel();
   	void load(bool pause=false);
	void save();
    void init();
    void term();
};

extern Engine engine;
#endif // _ENGINE_H_

