#include <getopt.h>
#include <stdio.h>
#include <stdbool.h>

#include "err.h"
#include "commands/commands.h"
#include "commands/upgrade/upgrade.h"
#include "repository/repository.h"

static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"yes", no_argument, NULL, 'y'},
    {"update", no_argument, NULL, 'u'},
    {NULL, 0, NULL, 0}
};

void print_upgrade_help(void) {
    putchar('\n');
    printf("Usage: spk upgrade [options]\n");
    printf("Upgrade all installed packages that have a newer version available\n");
    putchar('\n');
    printf("options:\n");
    printf("    --yes, -y       Automatically answer yes for all questions\n");
    printf("    --update, -u    Update repositories before checking for upgrades\n");
    printf("    --help, -h      Print this help message\n");
}

int upgrade_spk(int argc, char **argv) {
    int opt;

    bool assume_yes = false;

    while ((opt = getopt_long(argc, argv, "huy", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_upgrade_help();
                return ERR_OK;

            case 'y':
                assume_yes = true;
                break;

            case 'u':
                update_repositories();
                break;

            case '?':
                return ERR_EXIT;
        }
    }

    return upgrade_packages(assume_yes);
}