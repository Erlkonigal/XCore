#pragma once

#include <common.hpp>
#include <device.hpp>

class RealtimeClock : public Device {
    uint64_t bootTime;
public:
    RealtimeClock(std::string name, word_t low, word_t high);
    ~RealtimeClock();
    void read(word_t addr, word_t *data) override;
    void write(word_t addr, word_t data, word_t mask) override;
};