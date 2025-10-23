#pragma once

#include <common.hpp>

class Device {
protected:
    std::string name;
    word_t low, high;
    void *space;
public:
    virtual std::string getName() { return name; }
    virtual word_t getLow() { return low; }
    virtual word_t getHigh() { return high; }
    virtual void *getSpace() { return space; }
    virtual bool check(word_t addr) { return addr >= low && addr <= high; }
    virtual void read(word_t addr, word_t *data) = 0;
    virtual void write(word_t addr, word_t data, word_t mask) = 0;
    virtual ~Device() {}
};
