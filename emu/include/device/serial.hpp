#pragma once

#include <common.hpp>
#include <device.hpp>

class Serial: public Device {
public:
    Serial(std::string name, word_t low, word_t high);
    ~Serial();
    void read(word_t addr, word_t *data) override;
    void write(word_t addr, word_t data, word_t mask) override;
};