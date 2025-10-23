#pragma once

#include <common.hpp>
#include <riscv/context.hpp>
#include <dlfcn.h>

enum Direction {
    REF_TO_DUT,
    DUT_TO_REF
};

class DiffTest {
    Context ref, dut;
    bool is_skip_ref;
    int skip_dut_nr_instr;
    void (*ref_memcpy)(word_t addr, void *buf, size_t n, bool direction);
    void (*ref_regcpy)(void *dut, bool direction);
    void (*ref_exec)(uint64_t n);
    void (*ref_raise_intr)(uint64_t NO);
    void (*ref_init)(int port);
public:
    void init();
    void setReference(char *ref_so, int port);
    void skipRef();
    void skipDut(int nr_ref, int nr_dut);
    void step(word_t pc, word_t dnpc);
    void raiseIntr(uint64_t NO);
    void updateRefMem(word_t addr, word_t data);
    void updateRefRegs();
};

extern DiffTest difftest;