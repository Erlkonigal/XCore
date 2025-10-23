#pragma once

#include <common.hpp>
#include <readline/readline.h>
#include <readline/history.h>
#include <sdb/expr.hpp>
#include <sdb/wp.hpp>
#include <exec.hpp>
#include <functional>

struct Command {
    const char *name;
    const char *desc;
    std::function<int(const char *)> handler;
};

class SimpleDebugger {
    std::vector<Command> cmds;
public:
    bool batch;
    SimpleDebugger();
    int run(const char *args);
    int help(const char *args);
    int quit(const char *args);
    int stepInstr(const char *args);
    int info(const char *args);
    int print(const char *args);
    int scan(const char *args);
    int watch(const char *args);
    int del(const char *args);
    void mainloop();
};

extern SimpleDebugger sdb;