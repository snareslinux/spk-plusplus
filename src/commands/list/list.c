#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

#include "err.h"
#include "commands/commands.h"
#include "commands/list/list.h"

static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"ecplicit", no_argument, NULL, 'e'},
    {"long", no_argument, NULL, 'l'},
    {NULL, 0, NULL, 0}
};

void print_list_help(void) {
    putchar('\n');
    printf("Usage: spk list [options]\n");
    printf("List installed packages\n");
    putchar('\n');
    printf("options:\n");
    printf("    --explicit, -e   List only explicitly installed package\n");
    printf("    --long, -l       List using long format\n");
    printf("    --help, -h       Print this help message\n");
}

int list(int argc, char **argv) {
    int opt;

    bool explicit_only = false;
    bool long_format = false;

    while ((opt = getopt_long(argc, argv, "hle", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_list_help();
                return ERR_OK;

            case 'l':
                long_format = true;
                break;

            case 'e':
                explicit_only = true;
                break;
        }
    }

    return list_packages(explicit_only, long_format);
}