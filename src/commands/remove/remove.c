#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>

#include "err.h"
#include "commands/commands.h"
#include "commands/remove/remove.h"

static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"force", no_argument, NULL, 'f'},
    {"yes", no_argument, NULL, 'y'},
    {NULL, 0, NULL, 0}
};

void print_remove_help(void) {
    putchar('\n');
    printf("Usage: spk remove [options] PACKAGE\n");
    printf("Removes installed package or packages from your system\n");
    putchar('\n');
    printf("options:\n");
    printf("    --yes, -y   Automatically answer yes for all questions\n");
    printf("    --force, -f Force removal and ignore and dependencies\n");
    printf("    --help, -h  Print this help message\n");
}

int remove_spk(int argc, char **argv) {
    int opt;

    bool assume_yes = false;

    while ((opt = getopt_long(argc, argv, "hfy", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_remove_help();
                return ERR_OK;

            case 'f':
                break;

            case 'y':
                assume_yes = true;
                break;

            case '?':
                return ERR_EXIT;
        }
    }

    if (optind >= argc) {
        print_remove_help();
        return ERR_EXIT;
    }

    return remove_packages(argc - optind, argv + optind, assume_yes);
}