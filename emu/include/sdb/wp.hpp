#pragma once

#include <common.hpp>

#define NR_WATCHPOINTS 32
#define EXPR_MAX_LEN 128

struct WatchPoint {
    int No;
    WatchPoint *next;
    char expr[EXPR_MAX_LEN];
    word_t value;
};

class WatchPointPool {
    WatchPoint pool[NR_WATCHPOINTS];
    WatchPoint phead;
    WatchPoint pfree;
public:
    WatchPointPool();
    WatchPoint *newWatchPoint();
    WatchPoint *getHead();
    void freeWatchPoint(int No);
    void print();
};

extern WatchPointPool wpPool;