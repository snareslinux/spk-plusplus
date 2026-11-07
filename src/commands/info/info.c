#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

#include "err.h"
#include "commands/commands.h"
#include "commands/info/info.h"
#include "repository/repository.h"

static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"update", no_argument, NULL, 'u'},
    {"online", no_argument, NULL, 'o'},
    {"installed", no_argument, NULL, 'i'},
    {NULL, 0, NULL, 0}
};

void print_info_help(void) {
    putchar('\n');
    printf("Usage: spk info [options] PACKAGE\n");
    printf("Show package information\n");
    putchar('\n');
    printf("options:\n");
    printf("    --online, -o     Get package info from repository\n");
    printf("    --installed, -i  Get package info from a installed package\n");
    printf("    --update, -u     Update repositories before searching\n");
    printf("    --help, -h       Print this help message\n");
}

int info(int argc, char **argv) {
    int opt;

    bool installed = false;
    bool online = false;

    while ((opt = getopt_long(argc, argv, "houi", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_info_help();
                return ERR_OK;

            case 'o':
                online = true;
                break;

            case 'i':
                installed = true;
                break;

            case 'u':
                update_repositories();
                break;

            case '?':
                return ERR_EXIT;
        }
    }

    if (installed && online) {
        return ERR_EXIT;
    }

    int non_options = argc - optind;
    if (non_options != 1) {
        return ERR_EXIT;
    }

    return print_package_info(argv[optind], online, installed);
}