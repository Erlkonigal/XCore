#include <sdb/itrace.hpp>

ITrace itrace;

ITrace::ITrace() {
    init_disasm("riscv32");
}

std::string ITrace::disam(uint64_t pc, uint32_t instr) {
    std::string str;
    str.resize(256);
    disassemble(&str[0], 256, pc, (uint8_t *)&instr, 4);
    if (trace.size() == capacity) {
        trace.pop();
    }
    trace.push({pc, instr, str});
    return std::string(str);
}

void ITrace::print() {
    while (!trace.empty()) {
        auto instr = trace.front();
        printf("0x%08lx: 0x%08x\t%s\n", instr.pc, instr.instr, instr.disam.c_str());
        trace.pop();
    }
}