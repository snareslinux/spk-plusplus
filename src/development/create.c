#include <getopt.h>
#include <stdio.h>

#include "err.h"
#include "package/package.h"

static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"output", required_argument, NULL, 'o'},
    {NULL, 0, NULL, 0}
};

void print_create_help(void) {
    putchar('\n');
    printf("Usage: spk create [options] DIR\n");
    printf("Compress a Creates a spk file from the directorys content\n");
    putchar('\n');
    printf("options:\n");
    printf("    --output, -o    Specify where the output gets written\n");
    printf("    --help, -h      Print this help message\n");
}

int create_spk(int argc, char **argv) {
    int opt;

    optind = 1;

    const char *output = 0;

    while ((opt = getopt_long(argc, argv, "ho:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_create_help();
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
        return ERR_EXIT;
    }
    
    return write_spk(argv[optind], output);
}