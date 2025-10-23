#pragma once

#include "common.hpp"

struct CSRRegs {
    word_t mstatus;
    word_t mie;
    word_t mtvec;
    word_t mepc;
    word_t mcause;
    word_t satp;
};

struct Context {
    word_t reg[32];
    word_t pc;
    // mcsr
    word_t mtvec, mepc, mstatus, mcause; 
    word_t medeleg, mideleg;
    word_t mie, mip;
    // scsr
    word_t satp, scause, stvec, sepc; 
};
