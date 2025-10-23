#pragma once

#include <common.hpp>

enum ExecState{
    Stopped,
    Running,
    Finished,
    Aborted,
    GoodTrap,
    BadTrap
};

class SimExecutor {
protected:
    std::vector<std::string> regNames;
    int numOfRegs;
public:
    virtual int getNumOfRegs() { return numOfRegs; }
    virtual std::string getRegName(int i) { return regNames[i]; }
    virtual int getState() = 0;
    virtual void setState(int) = 0;
    virtual word_t getReg(const std::string, bool *success) = 0;
    virtual void getContext(void *context) = 0;
    virtual void stepOnce() = 0;
    virtual void reset() = 0;
    virtual void print() = 0;
    virtual void raiseIntr() = 0;
    virtual void lowerIntr() = 0;
    virtual ~SimExecutor() {};
};

extern SimExecutor *executor;