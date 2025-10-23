#include "SDL_events.h"
#include <device/keyboard.hpp>
#include <device/sdl.hpp>

Keyboard::Keyboard(std::string name, word_t low, word_t high) {
#ifdef CONFIG_ENABLE_KEYBOARD
    this->name = name;
    this->low = low;
    this->high = high;

    NEMU_KEYS(SDL_KEYMAP)

    auto handler = [&](const SDL_Event &event) {
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            bool is_down = event.type == SDL_KEYDOWN;
            uint32_t scancode = event.key.keysym.scancode;
            uint32_t am_scancode = keymap[scancode] | (is_down ? KEYDOWN_MASK : 0);
            keyQueue.push(am_scancode);
        }
    };
    sdl.addEventHandler(handler);
    Info("create keyboard %s: 0x%08x - 0x%08x", name.c_str(), low, high);
#endif
}

Keyboard::~Keyboard() {
#ifdef CONFIG_ENABLE_KEYBOARD
    Info("delete keyboard %s: 0x%08x - 0x%08x", this->name.c_str(), this->low, this->high);
#endif
}

void Keyboard::read(word_t addr, word_t *data) {
#ifdef CONFIG_ENABLE_KEYBOARD
    assert(this->check(addr));
    addr = addr & ~0x3;
    if (keyQueue.empty()) {
        *data = NEMU_KEY_NONE;
    } else {
        *data = keyQueue.front();
        keyQueue.pop();
    }
#endif
}

void Keyboard::write(word_t addr, word_t data, word_t mask) {
    assert(this->check(addr));
    // do nothing
}