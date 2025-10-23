#include <ic.hpp>
#include <exec.hpp>
#include <device/memory.hpp>

InterConnect ic;

InterConnect::InterConnect() {
    Memory *memory = new Memory("memory", CONFIG_MEM_BASE, CONFIG_MEM_BASE + CONFIG_MEM_SIZE - 1); // 128MB
    this->devices.push_back(memory);
}

InterConnect::~InterConnect() {
    for (auto device: this->devices) {
        delete device;
    }
}

void InterConnect::addDevice(Device *device) {
    this->devices.push_back(device);
}

void InterConnect::print() {
    for (auto device: this->devices) {
        printf("device %s: 0x%08x - 0x%08x\n", 
            device->getName().c_str(),
            device->getLow(),
            device->getHigh()
        );
    }
}

Device *InterConnect::getDevice(word_t addr) {
    for (auto device: this->devices) {
        if (device->check(addr)) {
            return device;
        }
    }
    return nullptr;
}

Device *InterConnect::getDevice(std::string name) {
    for (auto device: this->devices) {
        if (device->getName() == name) {
            return device;
        }
    }
    return nullptr;
}

void InterConnect::read(word_t addr, word_t *data) {
    Device *device = this->getDevice(addr);
    if (device) {
        device->read(addr, data);
        Debug("read device %s, addr = 0x%08x, rdata = 0x%08x", 
            device->getName().c_str(),
            addr, *data
        );
    }
    else {
        Error("read: invalid address 0x%08x", addr);
        *data = 0;
        executor->setState(Aborted);
    }
}

void InterConnect::write(word_t addr, word_t data, word_t mask) {
    Device *device = this->getDevice(addr);
    if (device) {
        device->write(addr, data, mask);
        Debug("write device %s, addr = 0x%08x, data = 0x%08x, mask = 0x%08x", 
            device->getName().c_str(),
            addr, data, mask
        );
    }
    else {
        Error("write: invalid address 0x%08x", addr);
        executor->setState(Aborted);
    }
}

extern "C" {
    int DPIC_Read(int addr) {
        uint32_t rdata;
        ic.read(addr, &rdata);
        return rdata;
    }
    void DPIC_Write(int addr, int data, int mask) {
        ic.write(addr, data, mask);
    }
}