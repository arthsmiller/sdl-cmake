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

enum Direction {
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
    int hitbox [4];
    int red, green, blue;
    SpriteType type;
    bool isMouseOverSprite;
    Direction currentAction;
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
    void spawnNewEnemy();

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
    TTF_Font * font;
    SDL_Color color;
    SDL_Surface * surface;
    SDL_Texture * texture;

    // sprite stuff
    boost::container::vector<Sprite> sprites;
    long long spawnCountdown;
};

class Helper {
public:
    static int randomInt(int min, int max);
};

class OverpassApiClient {
public:
    OverpassApiClient(const std::string& endpoint);
    ~OverpassApiClient();

    std::string executeQuery(const std::string& query);

private:
    struct MemoryStruct {
        char* memory;
        size_t size;

        MemoryStruct();
        ~MemoryStruct();
    };

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, MemoryStruct* output);

    CURL* curl_;
    std::string endpoint_;
};
