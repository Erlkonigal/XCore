#include <sdb.hpp>
#include <ic.hpp>
#include <exec.hpp>
#include <device/sdl.hpp>
#include <sdb/itrace.hpp>

SimpleDebugger sdb;

SimpleDebugger::SimpleDebugger() {
    #define CMD_LAMBDA(func) [this](const char *args) { return func(args); }
    Command c[] = {
        {"help", "Display this information", CMD_LAMBDA(help)},
        {"r", "Run the program", CMD_LAMBDA(run)},
        {"q", "Quit the debugger", CMD_LAMBDA(quit)},
        {"si", "Step one instruction", CMD_LAMBDA(stepInstr)},
        {"info", "Display information about registers", CMD_LAMBDA(info)},
        {"p", "Print the value of an expression", CMD_LAMBDA(print)},
        {"x", "Scan memory", CMD_LAMBDA(scan)},
        {"w", "Set a watchpoint", CMD_LAMBDA(watch)},
        {"d", "Delete a watchpoint", CMD_LAMBDA(del)},
    };
    cmds.assign(c, c + ARRAY_SIZE(c));
    batch = false;
    #undef CMD_LAMBDA
}

static int check_state() {
    switch (executor->getState()) {
        case Running:
        case Stopped:
            return 0;
        case Finished:{
            Info("Program exited");
            word_t exit_code = executor->getReg("a0", NULL);
            if (exit_code == 0) {
                executor->setState(GoodTrap);
                Info("Hit Good Trap");
            }
            else {
                executor->setState(BadTrap);
                executor->print();
                itrace.print();
                Error("Hit Bad Trap");
                
            }
            return exit_code;
        }
        case Aborted:
            executor->print();
            itrace.print();
            Error("Program aborted");
            return -1;
        default:
            assert(0);
            break;
    }
}

int SimpleDebugger::stepInstr(const char *args) {
    executor->stepOnce();
    int ret = check_state();
    if (ret != 0) {
        return ret;
    }
    executor->setState(Stopped);
    return 0;
}

int SimpleDebugger::run(const char *args) {
    executor->setState(Running);
    while (executor->getState() == Running) {
        executor->stepOnce();
    }
    return check_state();
}

int SimpleDebugger::help(const char *args) {
    char *arg = strtok(NULL, " ");
    int i;
    if (arg == NULL){
        /* no argument given */
        for (i = 0; i < cmds.size(); i++) {
            printf("%s - %s\n", cmds[i].name, cmds[i].desc);
        }
    }
    else {
        for (i = 0; i < cmds.size(); i++) {
            if (strcmp(arg, cmds[i].name) == 0) {
                printf("%s - %s\n", cmds[i].name, cmds[i].desc);
                return 0;
            }
        }
        printf("Unknown command '%s'", arg);
    }
    return 0;
}

int SimpleDebugger::quit(const char *args) {
    executor->setState(Finished);
    return 0;
}

int SimpleDebugger::info(const char *args) {
    if (!args) {
        Warn("No argument given");
        return 0;
    }
    switch (args[0]) {
        case 'r':
            executor->print();
            break;
        case 'w':
            wpPool.print();
            break;
        default:
            Error("Unknown argument '%c'", args[0]);
    }
    return 0;
}

int SimpleDebugger::print(const char *args) {
    if (!args){
        Warn("missing expression");
        return 0;
    }

    int mode = 0;
    char tmp[65536];
    char *target;
    char *arg_m = strtok(strcpy(tmp, args), " ");
    if (arg_m[0] == '/') {
        target = arg_m + strlen(arg_m) + 1;
        char *opt = arg_m + 1;
        while (*opt != '\0') {
            switch (*opt) {
                case 'x':
                    mode = 1;
                    break;
                case 's':
                    mode = 2;
                    break;
                default:
                    break;
            }
            opt += 1;
        }
    }
    else target = (char *)args;

    bool success = true;
    word_t ans = exprHandler.expr(target, &success);
    if (!success) Warn("Bad expression");

    switch (mode) {
        case 0:
            printf("%u\n", ans);
            break;
        case 1:
            printf("0x%x\n", ans);
            break;
        case 2:
            printf("%d\n", ans);
            break;
        default:
            break;
    }
    return 0;
}

int SimpleDebugger::scan(const char *args) {
    int N = 0;
    char tmp[65536];
    char mode = 'u';
    char width = 0;
    char *target;
    char *arg_m = strtok(strcpy(tmp, args), " ");
    if (arg_m[0] == '/') {
        target = arg_m + strlen(arg_m) + 1;
        char *opt = arg_m + 1;
        while (*opt != '\0') {
            if (*opt == 'x' && mode == 'u')
                mode = 'x';
            else if (*opt == 'b' && width == 0)
                width = 1;
            else if (*opt == 'h' && width == 0)
                width = 2;
            else if (*opt == 'w' && width == 0)
                width = 4;
            else if (*opt >= '0' && *opt <= '9' && N == 0) {
                while (*opt >= '0' && *opt <= '9') {
                    N = N * 10 + *opt - '0';
                    opt += 1;
                }
                continue;
            }
            else assert(0);
            opt += 1;
        }
    }
    else target = (char *)args;

    if (!target) {
        Warn("missing addr");
        return 0;
    }

    bool success = true;
    word_t ans = exprHandler.expr(target, &success);
    if (!success) Warn("Bad expression");

    if (!N)
        N = 1;
    if (!width)
        width = 4;
    char ctrl_width[8];
    char output[32];
    sprintf(ctrl_width, "0%d", width * 2);
    sprintf(output, "%s%%%s%c\n", mode == 'x' ? "0x" : "",
            mode == 'x' ? ctrl_width : "",
            mode);

    for (int i = 0; i < N; i++) {
        word_t addr = ans + width * i;
        word_t inst;
        ic.read(addr, &inst);
        word_t wid[5] = {0, 0xff, 0xffff, 0, 0xffffffff};

        printf("0x%08x:\t", ans + width * i);
        printf(output, (inst & wid[width]));
    }
    return 0;
}

int SimpleDebugger::watch(const char *args) {
#ifdef CONFIG_WATCHPOINT
    if (!args) {
        Warn("missing expr");
        return 0;
    }

    bool success = true;
    WatchPoint *wp_n = wpPool.newWatchPoint();
    if (wp_n == NULL) {
        Error("create wp failed");
        return 0;
    }
    strcpy(wp_n->expr, args);
    wp_n->value = exprHandler.expr(wp_n->expr, &success);
    if (!success) Warn("bad expression");

    printf("new watchpoint %d:%s\n", wp_n->No, wp_n->expr);
#endif
    return 0;
}

int SimpleDebugger::del(const char *args) {
#ifdef CONFIG_WATCHPOINT
    if (!args) {
        Warn("missing expr");
        return 0;
    }

    int No = atoi(args);
    wpPool.freeWatchPoint(No);
#endif
    return 0;
}

void SimpleDebugger::mainloop() {
#ifndef CONFIG_SDB
    assert(run(NULL) == 0);
    return;
#endif
    if (batch) {
        run(NULL);
        return;
    }
    auto rl_gets = [&]() {
        static char *line_read = nullptr;
        static char hist_buf[65536];
        if (line_read) {
            free(line_read);
            line_read = nullptr;
        }
        line_read = readline("(sdb) ");
        if (line_read && *line_read) {
            add_history(line_read);
        }
        else {
            HISTORY_STATE *hist = history_get_history_state();
            int offset = hist->offset;
            if (offset == 0) {
                return line_read;
            }
            char *line = hist->entries[offset - 1]->line;
            memset(hist_buf, 0, sizeof(hist_buf));
            strcpy(hist_buf, line);
            free(hist);
            return hist_buf;
        }
        return line_read;
    };

    for (char *str; (str = rl_gets()) != nullptr;) {
        char *str_end = str + strlen(str);
        char *cmd = strtok(str, " ");
        if (cmd == nullptr) continue;
        char *args = cmd + strlen(cmd) + 1;
        if (args >= str_end) args = nullptr;
        int i;
        for (i = 0; i < cmds.size(); i++) {
            if (strcmp(cmd, cmds[i].name) == 0) {
                if (cmds[i].handler(args) < 0) return;
                break;
            }
        }
        if (i == cmds.size()) {
            Warn("Unknown command '%s'", cmd);
        }
    }
}