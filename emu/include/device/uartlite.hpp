#pragma once

#include <device.hpp>
#include <queue>

class Queue {
    std::queue<char> q;
    int entry;
public:
    Queue(int entry) : entry(entry) {};
    bool full() { return q.size() >= entry; }
    bool empty() { return q.size() == 0; }
    char front() { return q.front(); }
    void pop() { q.pop(); }
    void push(char data) { q.push(data); }
};

class UARTLite : public Device {
    Queue txQueue;
    Queue rxQueue;
    bool enableInterrupt;
public:
    UARTLite(std::string name, word_t low, word_t high);
    ~UARTLite();
    void read(word_t addr, word_t *data);
    void write(word_t addr, word_t data, word_t mask);
};