#include <device/rtc.hpp>
#include <exec.hpp>
#include <chrono>

RealtimeClock::RealtimeClock(std::string name, word_t low, word_t high) {
    this->name = name;
    this->low = low;
    this->high = high;
    this->bootTime = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
    Info("create RTC %s: 0x%08x - 0x%08x", name.c_str(), low, high);
}

RealtimeClock::~RealtimeClock() {
    Info("delete RTC %s: 0x%08x - 0x%08x", this->name.c_str(), this->low, this->high);
}

void RealtimeClock::read(word_t addr, word_t *data) {
    assert(this->check(addr));
    addr = addr & ~0x3;
    word_t offset = (addr - this->low) >> 2;
    switch (offset) {
        case 0:
            *data = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count() - this->bootTime;
            break;
        default:
            Error("read: invalid address 0x%08x", addr);
            executor->setState(Aborted);
    }
}

void RealtimeClock::write(word_t addr, word_t data, word_t mask) {
    assert(this->check(addr));
    addr = addr & ~0x3;
    word_t offset = (addr - this->low) >> 2;
    switch (offset) {
        default:
            Error("write: invalid address 0x%08x", addr);
            executor->setState(Aborted);
    }
}