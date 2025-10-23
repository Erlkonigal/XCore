#include <cstdio>
#include <device/serial.hpp>
#include <exec.hpp>
#include <string>
#include <sdb.hpp>
#include <fcntl.h>
#include <unistd.h>

Serial::Serial(std::string name, word_t low, word_t high) {
    this->name = name;
    this->low = low;
    this->high = high;
    Info("create serial %s: 0x%08x - 0x%08x", name.c_str(), low, high);
}

Serial::~Serial() {
    Info("delete serial %s: 0x%08x - 0x%08x", this->name.c_str(), this->low, this->high);
}

void Serial::read(word_t addr, word_t *data) {
    assert(this->check(addr));
    addr = addr & ~0x3;
    char c;
    int fd = fileno(stdin);
    fcntl(fd, F_SETFL, O_NONBLOCK);
    if (::read(fd, &c, 1) == 1) *data = c;
    else *data = -1;
}

void Serial::write(word_t addr, word_t data, word_t mask) {
    assert(this->check(addr));
    addr = addr & ~0x3;
    char c = data;
    putchar(c);
    fflush(stdout);
}