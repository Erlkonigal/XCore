#include "device/sdl.hpp"
#include <device/video.hpp>

Video::Video(std::string name, word_t low, word_t high) {
#ifdef CONFIG_ENABLE_VIDEO
    this->name = name;
    this->low = low;
    this->high = high;
    this->space = new char[high - low + 1];
    memset(this->space, 0, high - low + 1);
    Info("create video %s: 0x%08x - 0x%08x", name.c_str(), low, high);
#endif
}

Video::~Video() {
#ifdef CONFIG_ENABLE_VIDEO
    delete (char *)this->space;
    Info("delete video %s: 0x%08x - 0x%08x", this->name.c_str(), this->low, this->high);
#endif
}

void Video::read(word_t addr, word_t *data) {
    assert(this->check(addr));
    word_t offset = addr - this->low;
    switch (offset) {
        case 0:
            *data = SCREEN_HEIGHT;
            break;
        case 2:
            *data = SCREEN_WIDTH << 16;
            break;
        default:
            *data = 0;
            break;
    }
}

void Video::write(word_t addr, word_t data, word_t mask) {
    assert(this->check(addr));
    word_t offset = addr - this->low;
    if (offset >= 0x10) {
        addr = addr & ~0x3;
        word_t *p = (word_t *)((char *)this->space + addr - this->low);
        *p = (*p & ~mask) | (data & mask);
    }
}