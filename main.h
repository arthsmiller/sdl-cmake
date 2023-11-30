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
};

enum SpriteType {
    PLAYER,
    ENEMY,
    DEFAULT,
};

class Sprite {
public:
    int x, y, width, height;
    SpriteType type;
    bool isMouseOverSprite;
    Sprite() :
        x(0),
        y(0),
        width(20),
        height(20),
        type(DEFAULT),
        isMouseOverSprite(false)
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
    void removeSprite(const Sprite& spriteToRemove);
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
    int randomInt(int min, int max);
};