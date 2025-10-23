#include <device/memory.hpp>

static const word_t img [] = {
    0x00000413, //          	li	s0,0
    0x00009117, //          	auipc	sp,0x9
    0xffc10113, //          	addi	sp,sp,-4 # 80009000 <_end>
    0x00c000ef, //          	jal	ra,80000018 <_trm_init>
    0x00000513, //          	li	a0,0
    0x00008067, //          	ret    
    0xff010113, //          	addi	sp,sp,-16
    0x00000517, //          	auipc	a0,0x0
    0x01c50513, //          	addi	a0,a0,28 # 80000038 <_etext>
    0x00112623, //          	sw	ra,12(sp)
    0xfe9ff0ef, //          	jal	ra,80000010 <main>
    0x00050513, //          	mv	a0,a0
    0x00100073, //          	ebreak
    0x0000006f, //          	j	80000034 <_trm_init+0x1c>
};

Memory::Memory(std::string name, word_t low, word_t high) {
    this->name = name;
    this->low = low;
    this->high = high;
    this->space = new char[high - low + 1];
    memset(this->space, 0, high - low + 1);
    Info("create memory %s: 0x%08x - 0x%08x", name.c_str(), low, high);
}

Memory::~Memory() {
    free(this->space);
    Info("delete memory %s: 0x%08x - 0x%08x", this->name.c_str(), this->low, this->high);
}

void Memory::read(word_t addr, word_t *data) {
    assert(this->check(addr));
    addr = addr & ~0x3;
    *data = *(word_t *)((char *)this->space + addr - this->low);
}

void Memory::write(word_t addr, word_t data, word_t mask) {
    assert(this->check(addr));
    addr = addr & ~0x3;
    word_t *p = (word_t *)((char *)this->space + addr - this->low);
    *p = (*p & ~mask) | (data & mask);
}

void Memory::import(const char *filename) {
    if (filename == nullptr) {
        Warn("filename is nullptr, import default image");
        memcpy(this->space, img, sizeof(img));
        capacity = sizeof(img);
        return;
    }
    Debug("import %s to memory %s", filename, this->name.c_str());
    FILE *fp = fopen(filename, "rb");
    word_t memsize = this->high - this->low + 1;
    assert(fread(this->space, 1, memsize, fp));

    fseek(fp, 0, SEEK_END);
    capacity = ftell(fp);
    fclose(fp);
    Info("import %s to memory %s: 0x%08x - 0x%08x, size = %lu", filename, this->name.c_str(), this->low, this->high, capacity);
    return;
}

