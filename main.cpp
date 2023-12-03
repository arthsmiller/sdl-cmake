// base stolen from https://github.com/andrew-lim/sdl2-boilerplate

#include <iostream>
#include <string>
#include <chrono>
#include <SDL.h>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <curl/curl.h>

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

    addSprite(PLAYER);

    for (int i = 0; i < Helper::randomInt(0, MAX_SPRITES); ++i)
        addSprite();

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
// DEBUG_printSpritesLocation(sprites);

    for (Sprite& sprite : sprites) {
        returnSpriteToCanvas(sprite);
        autoMoveSprite(sprite);

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
    sprite.x = Helper::randomInt(0, DISPLAY_WIDTH);
    sprite.y = Helper::randomInt(0, DISPLAY_HEIGHT);
}

void Game::addSprite(SpriteType type)
{
    Helper helper;

    sprites.emplace_back();

    moveSpriteToRandomPlace(sprites.back());

    sprites.back().type = type;

    sprites.back().red = Helper::randomInt(0, 255);
    sprites.back().green = Helper::randomInt(0, 255);
    sprites.back().blue = Helper::randomInt(0, 255);
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

void Game::autoMoveSprite(Sprite& sprite)
{
    if (sprite.type == PLAYER)
        return;

    Helper helper;

    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();

    if (sprite.actionEnd == 0 || sprite.actionEnd <= now) {
        sprite.currentAction = static_cast<Direction>(Helper::randomInt(0, 8));
        sprite.actionEnd = Helper::randomInt(0, 3000) + now;
    }
    else {
        switch (sprite.currentAction) {
            case STOP: break;
            case UP: sprite.y += HERO_SPEED; break;
            case RIGHT: sprite.x += HERO_SPEED; break;
            case DOWN: sprite.y -= HERO_SPEED; break;
            case LEFT: sprite.x -= HERO_SPEED; break;
            case UPRIGHT: sprite.y += HERO_SPEED; sprite.x += HERO_SPEED; break;
            case DOWNRIGHT: sprite.y -= HERO_SPEED; sprite.x += HERO_SPEED; break;
            case DOWNLEFT: sprite.y -= HERO_SPEED; sprite.x -= HERO_SPEED; break;
            case UPLEFT: sprite.y += HERO_SPEED; sprite.x -= HERO_SPEED; break;
            default: break;
        }
    }
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

//
// CURL CLASS DEFINITIONS
//
std::string Curl::sendPost(std::string url, std::string query) {
    std::string overpassQuery = R"(
        node(37.783333,-122.416667,37.793333,-122.406667); out body;
    )";

    std::string overpassEndpoint = "https://overpass-api.de/api/interpreter";

    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query.c_str());

        std::string responseData;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &Curl::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            curl_easy_cleanup(curl);
            curl_global_cleanup();

            return responseData;
        }

        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize libcurl." << std::endl;

        return "hi";
    }

    curl_global_cleanup();

    return "hi";
}

size_t Curl::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}
