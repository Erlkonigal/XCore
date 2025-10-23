#pragma once

#include <common.hpp>
#include <device.hpp>

class Memory: public Device {
    word_t capacity;
public:
    Memory(std::string name, word_t low, word_t high);
    ~Memory();
    void read(word_t addr, word_t *data) override;
    void write(word_t addr, word_t data, word_t mask) override;
    void import(const char *filename);
    uint64_t getCapacity() { return capacity; }
};