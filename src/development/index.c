#include <getopt.h>
#include <stdio.h>

#include "err.h"
#include "package/package.h"
#include "commands/commands.h"

static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"output", required_argument, NULL, 'o'},
    {NULL, 0, NULL, 0}
};

void print_index_help(void) {
    putchar('\n');
    printf("Usage: spk write-index [options] DIR\n");
    printf("Creates a index file for the repository in the directory\n");
    putchar('\n');
    printf("options:\n");
    printf("    --output, -o    Specify where the output gets written\n");
    printf("    --help, -h      Print this help message\n");
}

int write_index(int argc, char **argv) {
    int opt;

    optind = 1;

    const char *output = 0;

    while ((opt = getopt_long(argc, argv, "ho:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_index_help();
                return ERR_OK;

            case 'o':
                output = optarg;
                break;

            case ':':
                return ERR_EXIT;

            case '?':
                return ERR_EXIT;
        }
    }

    int non_options = argc - optind;
    if (non_options != 1) {
        print_index_help();
        return ERR_EXIT;
    }
    
    return write_indexfile(argv[optind], output);
}