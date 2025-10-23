#include "common.hpp"
#include "device/memory.hpp"
#include "exec.hpp"
#include <difftest.hpp>
#include <ic.hpp>

DiffTest difftest;

void DiffTest::setReference(char *ref_so, int port) {
    assert(ref_so != nullptr);

    void *handle = dlopen(ref_so, RTLD_LAZY);
    assert(handle != nullptr);

    ref_memcpy = (void (*)(word_t, void *, size_t, bool))dlsym(handle, "difftest_memcpy");
    assert(ref_memcpy != nullptr);

    ref_regcpy = (void (*)(void *, bool))dlsym(handle, "difftest_regcpy");
    assert(ref_regcpy != nullptr);

    ref_exec = (void (*)(uint64_t))dlsym(handle, "difftest_exec");
    assert(ref_exec != nullptr);

    ref_raise_intr = (void (*)(uint64_t))dlsym(handle, "difftest_raise_intr");
    assert(ref_raise_intr != nullptr);

    ref_init = (void (*)(int))dlsym(handle, "difftest_init");
    assert(ref_init != nullptr);

    Info("Differential testing: ON");
    Info("The result of every instruction will be compared with %s. "
      "This will help you a lot for debugging, but also significantly reduce the performance. "
      "If it is not necessary, you can turn it off in menuconfig.", ref_so);

    ref_init(port);
}

void DiffTest::init() {
    Memory *mem = (Memory *)ic.getDevice("memory");
    executor->getContext(&dut);
    ref_memcpy(mem->getLow(), mem->getSpace(), mem->getCapacity(), DUT_TO_REF);
    ref_regcpy(&dut, DUT_TO_REF);
}

void DiffTest::skipRef() {
    is_skip_ref = true;
    skip_dut_nr_instr = 0;
}

void DiffTest::skipDut(int nr_ref, int nr_dut) {
    skip_dut_nr_instr += nr_dut;
    while (nr_ref-- > 0) {
        ref_exec(1);
    }
}

// #define checkCSR(reg) if (dut.csr.reg != ref.csr.reg) { \
//     Error("CSR mismatch: %s: 0x%x (ref) vs 0x%x (dut) at pc: 0x%x", #reg, ref.csr.reg, dut.csr.reg, ref.pc); \
//     executor->setState(Aborted); \
// } 

void DiffTest::step(word_t pc, word_t dnpc) {
    auto printRef = [&]() {
        Info("ref.pc = 0x%x", ref.pc);
        for (int i = 1; i < executor->getNumOfRegs(); i++) {
            Info("%s: 0x%x", executor->getRegName(i).c_str(), ref.reg[i]);
        }
        Info("mstatus = 0x%x", ref.mstatus);
        Info("mcause = 0x%x", ref.mcause);
        Info("mtvec = 0x%x", ref.mtvec);
        Info("mepc = 0x%x", ref.mepc);
        Info("medeleg = 0x%x", ref.medeleg);
        Info("mideleg = 0x%x", ref.mideleg);
        Info("mie = 0x%x", ref.mie);
        Info("mip = 0x%x", ref.mip);

        
        Info("satp = 0x%x", ref.satp);
        Info("scause = 0x%x", ref.scause);
        Info("stvec = 0x%x", ref.stvec);
        Info("sepc = 0x%x", ref.sepc);
    };
    auto check = [&]() {
        bool isSuccess = true;
        if (dnpc != ref.pc) {
            Error("PC mismatch: 0x%x (ref) vs 0x%x (dut)", ref.pc, dnpc);
            isSuccess = false;
            executor->setState(Aborted);
        }
        for (int i = 1; i < executor->getNumOfRegs(); i++) {
            if (dut.reg[i] != ref.reg[i]) {
                Error("Register mismatch: %s: 0x%x (ref) vs 0x%x (dut) at pc: 0x%x", executor->getRegName(i).c_str(), ref.reg[i], dut.reg[i], dnpc);
                isSuccess = false;
                executor->setState(Aborted);
            }
        }
        if (!isSuccess) printRef();
        return;
    };
    executor->getContext(&dut);
    if (skip_dut_nr_instr > 0) {
        ref_regcpy(&ref, REF_TO_DUT);
        if (ref.pc == dnpc) {
            skip_dut_nr_instr = 0;
            check();
            return;
        }
        skip_dut_nr_instr--;
        if (skip_dut_nr_instr == 0) {
            Error("Cannot catch up with the reference with ref.pc = 0x%x, dut.pc = 0x%x", ref.pc, pc);
            executor->setState(Aborted);
        }
        return;
    }

    if (is_skip_ref) {
        ref_regcpy(&dut, DUT_TO_REF);
        is_skip_ref = false;
        return;
    }

    ref_exec(1);
    ref_regcpy(&ref, REF_TO_DUT);

    check();
}

void DiffTest::raiseIntr(uint64_t NO) {
    ref_raise_intr(NO);
}

void DiffTest::updateRefMem(word_t addr, word_t data) {
    ref_memcpy(addr, &data, sizeof(word_t), DUT_TO_REF);
}

void DiffTest::updateRefRegs() {
    ref_regcpy(&dut, DUT_TO_REF);
}