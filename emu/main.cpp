#include "sdb/expr.hpp"
#include <common.hpp>
#include <sdb.hpp>
#include <ic.hpp>
#include <exec.hpp>
#include <difftest.hpp>
#include <riscv/npc/npc.hpp>
#include <device/memory.hpp>
#include <device/sdl.hpp>
#include <getopt.h>
#include <thread>

int argsHandler(int argc, char *argv[]) {
	const struct option table[] = {
		{"log", required_argument, NULL, 'l'},
		{"elf", required_argument, NULL, 'e'},
		{"batch", no_argument, NULL, 'b'},
		{"help", no_argument, NULL, 'h'},
		{"diff"     , required_argument, NULL, 'd'},
    	{"port"     , required_argument, NULL, 'p'},
		{0, 0, NULL, 0},
	};

    char *img_file = nullptr;
    char *log_file = nullptr;
    char *img_elf = nullptr;
    char *diff_so_file = nullptr;
    int difftest_port = 1234;

	int o;
	while ((o = getopt_long(argc, argv, "-bhe:d:p:l:", table, NULL)) != -1) {
		switch (o) {
            case 'l': 
                log_file = optarg;
                break;
            case 'p': 
                sscanf(optarg, "%d", &difftest_port); 
                break;
            case 'd': 
                diff_so_file = optarg; 
                break;
            case 'b':
                // sdb.batch = true;
                break;
            case 'e':
                img_elf = optarg;
                break;
            case 1: {
                img_file = optarg;
                break;
            }
            default:
                printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
                printf("\t-b,--batch              run with batch mode\n");
                printf("\t-e,--elf=FILE           read elf from FILE\n");
                printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
                printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
                printf("\t-l,--log=FILE           output log to FILE\n");
                printf("\n");
                exit(0);
		}
	}

    logger.setLogFile(log_file);
    ((Memory *)ic.getDevice("memory"))->import(img_file);

#ifdef CONFIG_DIFFTEST
    difftest.setReference(diff_so_file, difftest_port);
#endif
	return 0;
}

int main(int argc, char *argv[]) {
    argsHandler(argc, argv);
    executor = new NPCExecutor(argc, argv);
    executor->reset();
    exprHandler.setExecutor(executor);
#ifdef CONFIG_DIFFTEST
    difftest.init();
#endif
    std::thread([&](){sdb.mainloop();}).detach();
    sdl.run();
    int state = executor->getState();
    delete executor;
    if (state == GoodTrap) return 0;
    else return 1;
}