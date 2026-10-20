#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "err.h"
#include "commands/commands.h"
#include "commands/install/install.h"

static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"yes", no_argument, NULL, 'y'},
    {NULL, 0, NULL, 0}
};

void print_install_help(void) {
    putchar('\n');
    printf("Usage: spk install [options] PACKAGE\n");
    printf("Install a package or packages to your system\n");
    putchar('\n');
    printf("options:\n");
    printf("    --yes, -y       Automatically answer yes for all questions\n");
    printf("    --help, -h      Print this help message\n");
}

int install_spk(int argc, char **argv) {
    int opt;

    bool assume_yes = false;

    while ((opt = getopt_long(argc, argv, "hy", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_install_help();
                return ERR_OK;

            case 'y':
                assume_yes = true;
                break;
        }
    }

    return install_packages(argc - optind, argv + optind, assume_yes);
}