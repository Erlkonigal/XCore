#include <device/uartlite.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <exec.hpp>

UARTLite::UARTLite(std::string name, word_t low, word_t high): txQueue(16), rxQueue(16) {
    this->name = name;
    this->low = low;
    this->high = high;
    this->space = nullptr;
    this->enableInterrupt = false;

    Info("create uartlite %s: 0x%08x - 0x%08x", name.c_str(), low, high);
}

UARTLite::~UARTLite() {
    Info("delete uartlite %s: 0x%08x - 0x%08x", this->name.c_str(), this->low, this->high);
}

void UARTLite::read(word_t addr, word_t *data) {
    char c;
    int fd = fileno(stdin);
    fcntl(fd, F_SETFL, O_NONBLOCK);
    if (::read(fd, &c, 1) == 1) {
        // if (rxQueue.empty() && enableInterrupt) {
        //     executor->raiseIntr();
        // }
        rxQueue.push(c);
    }

    int offset = addr - this->low;
    switch (offset) {
        case 0x0:
            if (rxQueue.empty()) {
                *data = 0;
                break;
            }
            *data = rxQueue.front();
            rxQueue.pop();
            break;
        case 0x8: {
            char status = 0;
            if (!rxQueue.empty()) { // Rx FIFO Valid Data
                status |= 0x1;
            }
            if (rxQueue.full()) { // Rx FIFO Full
                status |= 0x2;
            }
            if (txQueue.empty()) { // Tx FIFO Empty
                status |= 0x4;
            }
            if (txQueue.full()) { // Tx FIFO Full
                status |= 0x8;
            }
            if (enableInterrupt) { // Interrupt Enable
                status |= 0x10;
            }
            *data = status;
            break;
        }
        default:
            Warn("UARTLite: read invalid address 0x%lx", addr);
            break;
    }
}

void UARTLite::write(word_t addr, word_t data, word_t mask) {
    int offset = addr - this->low;
    switch (offset) {
        case 0x4:
            txQueue.push(data);
            break;
        case 0xc:
            if (data & 0x1) { // reset tx
                txQueue = Queue(16);
            }
            if (data & 0x2) { // reset rx
                rxQueue = Queue(16);
            }
            if (data & 0x10) {
                enableInterrupt = true;
            }
            else {
                enableInterrupt = false;
            }
            break;
        default:
            Warn("UARTLite: write invalid address 0x%lx", addr);
            break;
    }

    if (!txQueue.empty()) {
        while (!txQueue.empty()) {
            char c = txQueue.front();
            txQueue.pop();
            putchar(c);
        }
        fflush(stdout);
        // executor->raiseIntr();
    }
}