#pragma once

#include <common.hpp>
#include <device.hpp>
#include <SDL2/SDL.h>

#define VGACTL_ADDR 0x21000000
#define FB_ADDR (VGACTL_ADDR + 0x10)

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 0
#endif

#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT 0
#endif


class Video: public Device {
public:
    Video(std::string name, word_t low, word_t high);
    ~Video();
    void read(word_t addr, word_t *data) override;
    void write(word_t addr, word_t data, word_t mask) override;
};