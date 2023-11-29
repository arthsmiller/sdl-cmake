// base stolen from https://github.com/andrew-lim/sdl2-boilerplate

#include <iostream>
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
    run();
}

void Game::draw() {
    SDL_Rect spriteRect;

    // Clear screen
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Render hero
    spriteRect.x = sprite.x;
    spriteRect.y = sprite.y;
    spriteRect.w = sprite.width;
    spriteRect.h = sprite.height;
    fillRect(&spriteRect, 255, 0, 0);

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
    sprintf_s(szFps, "%s: %d FPS", "SDL2 Base C++ - Use Arrow Keys to Move", fps);
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
                    onMouseOnSprite(sprite, event.button);
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
    returnSpriteToCanvas(sprite);

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
    else if (clicks[SDL_BUTTON_LMASK] && isMouseOverSprite) {
        moveSpriteToRandomPlace(sprite);
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

void Game::onMouseOnSprite(Sprite& sprite, SDL_MouseButtonEvent& mouseButton)
{
    if (mouseButton.x >= sprite.x && mouseButton.x <= sprite.x + sprite.width &&
        mouseButton.y >= sprite.y && mouseButton.y <= sprite.y + sprite.height
        ){
        isMouseOverSprite = true;

        return;
    }

    isMouseOverSprite = false;
}

void Game::moveSpriteToRandomPlace(Sprite& sprite)
{
    Helper helper;
std::cout << "BEGIN RAND" << std::endl;
std::cout << sprite.x << std::endl;
    sprite.x = helper.randomInt(0, DISPLAY_WIDTH);
    sprite.y = helper.randomInt(0, DISPLAY_HEIGHT);
    std::cout << "END RAND" << std::endl;
}

//
// HELPER CLASS DEFINITIONS
//
int Helper::randomInt(const int min, const int max) {
    const std::time_t now = std::time(nullptr);
    boost::random::mt19937 rng(static_cast<unsigned>(now));

    const boost::random::uniform_int_distribution<> generate(min, max);

    return generate(rng);
}
