#pragma once

#include <SDL2/SDL.h>
#include <common.hpp>
#include <device.hpp>
#include <atomic>
#include <functional>

#ifdef CONFIG_SCREEN_800X600
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#endif

#ifdef CONFIG_SCREEN_400X300
#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 300
#endif


using EventHandler = std::function<void(const SDL_Event &event)>;

class SDL_Screen {
    std::atomic<bool> terminate;
    std::vector<EventHandler> eventHandlers;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
public:
    SDL_Screen();
    void addEventHandler(EventHandler handler);
    void run();
};

extern SDL_Screen sdl;