// base stolen from https://github.com/andrew-lim/sdl2-boilerplate

#include <iostream>
#include <chrono>
#include <SDL.h>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>

#include "main.h"

int main(int argc, char *argv[]) {
    Game game;

    game.start();

    return 0;
}

//
// GAME CLASS DEFINITIONS
//
Game::Game()
    :frameSkip(0), running(0), window(NULL), renderer(NULL) {
}

Game::~Game() {
    this->stop();
}

void Game::start() {
    int flags = SDL_WINDOW_SHOWN;

    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        return;
    }

    if (SDL_CreateWindowAndRenderer(DISPLAY_WIDTH, DISPLAY_HEIGHT, flags, &window, &renderer)) {
        return;
    }

    this->running = 1;

    addSprite();
    addSprite();
    addSprite(PLAYER);

    run();
}

void Game::draw() {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    for (const auto& sprite : sprites) {
        SDL_Rect spriteRect;

        spriteRect.x = sprite.x;
        spriteRect.y = sprite.y;
        spriteRect.w = sprite.width;
        spriteRect.h = sprite.height;
        fillRect(&spriteRect, sprite.red, sprite.green, sprite.blue);
    }

    SDL_RenderPresent(renderer);
}

void Game::stop() {
    if (NULL != renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (NULL != window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    SDL_Quit();
}

void Game::fillRect(SDL_Rect* rc, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, rc);
}

void Game::fpsChanged(int fps) {
    char szFps[128];
    sprintf_s(szFps, "%s: %d FPS, %zd Sprites", "SDL2 Base C++", fps, sprites.size());
    SDL_SetWindowTitle(window, szFps);
}

void Game::onQuit() {
    running = 0;
}

void Game::run() {
    Uint64 past = SDL_GetTicks64();
    Uint64 now = past, pastFps = past;
    int fps = 0, framesSkipped = 0;

    SDL_Event event;

    while (running) {
        Uint64 timeElapsed = 0;

        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:    onQuit();            break;
                case SDL_KEYDOWN: onKeyDown(&event); break;
                case SDL_KEYUP:   onKeyUp(&event);   break;
                case SDL_MOUSEBUTTONDOWN:
                    onMouseDown(event.button);
                    onMouseOnSprite(sprites, event.button);
                break;
                case SDL_MOUSEBUTTONUP: onMouseUp(); break;
                case SDL_MOUSEMOTION:
                    break;
            }
        }

        // update/draw
        timeElapsed = (now = SDL_GetTicks64()) - past;

        if (timeElapsed >= UPDATE_INTERVAL) {
            past = now;
            update();

            if (framesSkipped++ >= frameSkip) {
                draw();
                ++fps;
                framesSkipped = 0;
            }
        }

        // fps
        if (now - pastFps >= 1000) {
            pastFps = now;
            fpsChanged(fps);
            fps = 0;
        }

        // sleep?
        SDL_Delay(1);
    }
}

void Game::update() {
DEBUG_printSpritesLocation(sprites);

    for (Sprite& sprite : sprites) {
        returnSpriteToCanvas(sprite);

        if (sprite.type == PLAYER) {
            if (keys[SDLK_LEFT]) {
                sprite.x -= HERO_SPEED;
            }
            else if (keys[SDLK_RIGHT]) {
                sprite.x += HERO_SPEED;
            }
            else if (keys[SDLK_UP]) {
                sprite.y -= HERO_SPEED;
            }
            else if (keys[SDLK_DOWN]) {
                sprite.y += HERO_SPEED;
            }
        }

        if (clicks[SDL_BUTTON_LMASK] && sprite.isMouseOverSprite) {
            moveSpriteToRandomPlace(sprite);
            sprite.isMouseOverSprite = false;
        }
    }
}

void Game::onKeyDown(SDL_Event* evt) {
    keys[evt->key.keysym.sym] = 1;
}

void Game::onKeyUp(SDL_Event* evt) {
    keys[evt->key.keysym.sym] = 0;
}

void Game::onMouseDown(SDL_MouseButtonEvent& mouseEvent)
{
    if (mouseEvent.button == SDL_BUTTON_LEFT)
    {
        clicks[SDL_BUTTON_LMASK] = 1;
    }
}

void Game::onMouseUp()
{
    clicks[SDL_BUTTON_LMASK] = 0;
    clicks[SDL_BUTTON_MMASK] = 0;
    clicks[SDL_BUTTON_RMASK] = 0;
    clicks[SDL_BUTTON_X1MASK] = 0;
    clicks[SDL_BUTTON_X2MASK] = 0;
}

void Game::returnSpriteToCanvas(Sprite& sprite)
{
    if (sprite.x > DISPLAY_WIDTH){
        sprite.x = 0;
    }
    else if (sprite.x < 0){
        sprite.x = DISPLAY_WIDTH;
    }
    else if (sprite.y > DISPLAY_HEIGHT){
        sprite.y = 0;
    }
    else if (sprite.y < 0){
        sprite.y = DISPLAY_HEIGHT;
    }
}

void Game::onMouseOnSprite(boost::container::vector<Sprite>& sprites, SDL_MouseButtonEvent& mouseButton)
{
    for (Sprite& sprite : sprites) {
        if (mouseButton.x >= sprite.x && mouseButton.x <= sprite.x + sprite.width &&
            mouseButton.y >= sprite.y && mouseButton.y <= sprite.y + sprite.height
            ){
            sprite.isMouseOverSprite = true;

            return;
        }

        sprite.isMouseOverSprite = false;
    }
}

void Game::moveSpriteToRandomPlace(Sprite& sprite)
{
    Helper helper;

    sprite.x = helper.randomInt(0, DISPLAY_WIDTH);
    sprite.y = helper.randomInt(0, DISPLAY_HEIGHT);
}

void Game::addSprite(SpriteType type)
{
    Helper helper;

    sprites.emplace_back();

    moveSpriteToRandomPlace(sprites.back());

    sprites.back().type = type;

    sprites.back().red = helper.randomInt(0, 255);
    sprites.back().green = helper.randomInt(0, 255);
    sprites.back().blue = helper.randomInt(0, 255);
}

void Game::removeSprite(Sprite& sprite)
{
    sprites.erase(
        std::remove_if(
            sprites.begin(),
            sprites.end(),
            [&sprite](const Sprite& lastSprite) {
                return &lastSprite == &sprite;
            }
        ),
        sprites.end()
    );
}

void Game::changeSpriteColor(Sprite& sprite, int red, int green, int blue)
{
    sprite.red = red;
    sprite.green = green;
    sprite.blue = blue;
}

void Game::DEBUG_printSpritesLocation(boost::container::vector<Sprite>& sprites)
{
    std::string type;

    for (const auto& sprite : sprites) {
        switch (sprite.type) {
            case PLAYER: type = "player"; break;
            case ENEMY: type = "enemy"; break;
            default: type = "default"; break;
        }

        std::cout << type << ": " << sprite.x << " " << sprite.y << std::endl;
    }
}

//
// HELPER CLASS DEFINITIONS
//
int Helper::randomInt(const int min, const int max) {
    const auto now = std::chrono::high_resolution_clock::now();
    const auto seed = static_cast<unsigned>(now.time_since_epoch().count());

    boost::random::mt19937 rng(seed);
    boost::random::uniform_int_distribution<> generate(min, max);

    return generate(rng);
}
