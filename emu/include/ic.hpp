#pragma once

#include "device.hpp"
#include <common.hpp>

class InterConnect {
    std::vector<Device *> devices;
public:
    InterConnect();
    ~InterConnect();
    void print();
    void read(word_t addr, word_t *data);
    void write(word_t addr, word_t data, word_t mask);
    void addDevice(Device *device);
    Device *getDevice(word_t addr);
    Device *getDevice(std::string name);
};

extern InterConnect ic;