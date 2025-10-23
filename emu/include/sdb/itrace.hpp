#pragma once

#include <common.hpp>
#include <queue>

struct Instr {
    uint64_t pc;
    uint32_t instr;
    std::string disam;
};

class ITrace {
    int capacity = 50;
    std::queue<Instr> trace;
public:
    ITrace();
    std::string disam(uint64_t pc, uint32_t instr);
    void print();
};

extern ITrace itrace;

// utils/disam.cpp
void init_disasm(const char *triple);
void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);