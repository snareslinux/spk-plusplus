#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "err.h"
#include "commands/commands.h"
#include "install.h"

static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"from-file", no_argument, NULL, 'f'},
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
    printf("    --from-file, -f Install package from local package file\n");
    printf("    --help, -h      Print this help message\n");
}

int install_spk(int argc, char **argv) {
    int opt;

    bool from_file = false;
    bool assume_yes = false;

    while ((opt = getopt_long(argc, argv, "hfy", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_install_help();
                break;

            case 'f':
                from_file = true;
                break;

            case 'y':
                assume_yes = true;
                break;
        }
    }

    return ERR_OK;
}