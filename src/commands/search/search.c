
#include <getopt.h>
#include <stdio.h>

#include "err.h"
#include "commands/search/search.h"
#include "repository/repository.h"

static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"update", no_argument, NULL, 'u'},
    {NULL, 0, NULL, 0}
};

void print_search_help(void) {
    putchar('\n');
    printf("Usage: spk search [options] PACKAGE\n");
    printf("Search for available package\n");
    putchar('\n');
    printf("options:\n");
    printf("    --update, -u     Update repositories before searching\n");
    printf("    --help, -h       Print this help message\n");
}

int search(int argc, char **argv) {
    int opt;

    while ((opt = getopt_long(argc, argv, "hu", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_search_help();
                return ERR_OK;

            case 'u':
                update_repositories();
                break;

            case '?':
                return ERR_EXIT;
        }
    }

    if (optind >= argc) {
        print_search_help();
        return ERR_EXIT;
    }

    return search_for_keyword(argv[optind]);
}