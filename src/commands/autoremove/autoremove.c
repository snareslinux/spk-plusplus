#include <stdbool.h>
#include <stdio.h>
#include <getopt.h>

#include "err.h"
#include "commands/autoremove/autoremove.h"

static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"yes", no_argument, NULL, 'y'},
    {NULL, 0, NULL, 0}
};

void print_autoremove_help(void) {
    putchar('\n');
    printf("Usage: spk autoremove [options]\n");
    printf("Remove all unneeded packages originally installed as dependencies.\n");
    putchar('\n');
    printf("options:\n");
    printf("    --yes, -y   Automatically answer yes for all questions\n");
    printf("    --help, -h  Print this help message\n");
}

int autoremove(int argc, char **argv) {
    int opt;
    bool assume_yes = false;

    while ((opt = getopt_long(argc, argv, "hy", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_autoremove_help();
                return ERR_OK;

            case 'f':
                break;

            case 'y':
                assume_yes = true;
                break;
        }
    }

    perform_autoremove(assume_yes);
    return ERR_OK;
}