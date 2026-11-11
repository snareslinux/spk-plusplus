#include <getopt.h>
#include <stdio.h>

#include "err.h"
#include "package/package.h"

static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"root", required_argument, NULL, 'R'},
    {"log", required_argument, NULL, 'l'},
    {NULL, 0, NULL, 0}
};

void print_extract_help(void) {
    putchar('\n');
    printf("Usage: spk extract [options] FILE\n");
    printf("Extracts a spk file into root\n");
    putchar('\n');
    printf("options:\n");
    printf("    --root, -R      Use another directory as root instead of /\n");
    printf("    --log, -l       Log all files extracted to a file\n");
    printf("    --help, -h      Print this help message\n");
}

int extract_spk(int argc, char **argv) {
    int opt;

    optind = 1;

    char *root = NULL;
    char *log = NULL;

    while ((opt = getopt_long(argc, argv, "hR:l:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_extract_help();
                return ERR_OK;

            case 'R':
                root = optarg;
                break;

            case 'l':
                log = optarg;
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
    
    return perform_extract(argv[optind], log ? root : "/", log);
}