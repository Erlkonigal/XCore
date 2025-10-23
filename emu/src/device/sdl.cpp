#include <device/sdl.hpp>
#include <device/video.hpp>
#include <ic.hpp>
#include <exec.hpp>

SDL_Screen sdl;

SDL_Screen::SDL_Screen() {
#ifdef CONFIG_ENABLE_SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        Error("SDL_Init failed: %s", SDL_GetError());
        exit(1);
    }
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (texture == NULL) {
        Error("SDL_CreateTexture failed: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    eventHandlers.push_back([](const SDL_Event &event) {
        if (event.type == SDL_QUIT) {
            sdl.terminate = true;
            if (executor) executor->setState(Finished);
        }
    });

    terminate = false;
#endif
}

void SDL_Screen::addEventHandler(EventHandler handler) {
    eventHandlers.push_back(handler);
}

void SDL_Screen::run() {
    while (true) {
        if (executor->getState() >= Finished || terminate) break;
#ifdef CONFIG_ENABLE_SDL
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            for (auto handler: eventHandlers) {
                handler(event);
            }
        }
#ifdef CONFIG_ENABLE_VIDEO
        Video *video = (Video *)ic.getDevice("video");
        char *space = (char *)video->getSpace();
        SDL_UpdateTexture(texture, NULL, space + 0x10, SCREEN_WIDTH * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
#endif
        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / 60);
#endif
    }
#ifdef CONFIG_ENABLE_SDL
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
#endif
}