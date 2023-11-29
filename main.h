//
// Created by arthur on 28.11.2023.
//

#ifndef MAIN_H
#define MAIN_H
#include <map>

#endif //MAIN_H

enum {
    DISPLAY_WIDTH = 480,
    DISPLAY_HEIGHT = 320,
    UPDATE_INTERVAL = 1000 / 60,
    HERO_SPEED = 2
};

class Sprite {
public:
    int x, y, width, height;
    Sprite() :
        x(0),
        y(0),
        width(20),
        height(20)
    {}
};

class Game {
public:
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
    void returnSpriteToCanvas(Sprite& sprite);
    void onMouseOnSprite(Sprite& sprite, SDL_MouseButtonEvent& mouseButton);
    void moveSpriteToRandomPlace(Sprite& sprite);
private:
    std::map<int, int> keys; // No SDLK_LAST. SDL2 migration guide suggests std::map
    std::map<int, int> clicks;
    bool isMouseOverSprite;
    int frameSkip;
    int running;
    SDL_Window* window;
    SDL_Renderer* renderer;
    Sprite sprite;
};

class Helper {
public:
    int randomInt(int min, int max);
};