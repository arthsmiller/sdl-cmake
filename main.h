//
// Created by arthur on 28.11.2023.
//

#ifndef MAIN_H
#define MAIN_H
#include <map>
#include <boost/container/vector.hpp>

#endif //MAIN_H

enum {
    DISPLAY_WIDTH = 480,
    DISPLAY_HEIGHT = 320,
    UPDATE_INTERVAL = 1000 / 60,
    HERO_SPEED = 2,
    MAX_SPRITES = 1000,
};

enum SpriteType {
    PLAYER,
    ENEMY,
    DEFAULT,
};

enum SpriteAction {
    STOP,
    UP,
    RIGHT,
    DOWN,
    LEFT,
    UPRIGHT,
    DOWNRIGHT,
    DOWNLEFT,
    UPLEFT,
};

class Sprite {
public:
    int x, y;
    int width, height;
    int red, green, blue;
    SpriteType type;
    bool isMouseOverSprite;
    SpriteAction currentAction;
    Uint64 actionEnd;
    Sprite() :
        x(0),
        y(0),
        width(20),
        height(20),
        red(255),
        green(0),
        blue(0),
        type(DEFAULT),
        isMouseOverSprite(false),
        currentAction(STOP),
        actionEnd(0)
    {}
};

class Game {
public:
    // engine stuff
    Game();
    ~Game();
    void start();
    void stop();
    void draw();
    void fillRect(SDL_Rect* rc, int r, int g, int b);
    void fpsChanged(int fps);
    void onQuit();
    void onKeyDown(SDL_Event* event);
    void onKeyUp(SDL_Event* event);
    void onMouseDown(SDL_MouseButtonEvent& mouseEvent);
    void onMouseUp();
    void run();
    void update();

    // sprite stuff
    void returnSpriteToCanvas(Sprite& sprite);
    void onMouseOnSprite(boost::container::vector<Sprite>& sprites, SDL_MouseButtonEvent& mouseButton);
    void moveSpriteToRandomPlace(Sprite& sprite);
    void addSprite(SpriteType type = DEFAULT);
    void removeSprite(Sprite& sprite);
    void changeSpriteColor(Sprite& sprite, int red, int green, int blue);
    void autoMoveSprite(Sprite& sprite);

    // DEBUG shit
    void DEBUG_printSpritesLocation(boost::container::vector<Sprite>& sprites);

private:
    // engine stuff
    std::map<int, int> keys;
    std::map<int, int> clicks;
    int frameSkip;
    int running;
    SDL_Window* window;
    SDL_Renderer* renderer;

    // sprite stuff
    boost::container::vector<Sprite> sprites;
};

class Helper {
public:
    static int randomInt(int min, int max);
};