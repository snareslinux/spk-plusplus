#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "err.h"
#include "path.h"
#include "util/util.h"
#include "ui/ui.h"
#include "commands/commands.h"
#include "commands/setup/setup.h"

static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"repository", required_argument, NULL, 'r'},
    {"yes", no_argument, NULL, 'y'},
    {"root", required_argument, NULL, 'R'},
    {NULL, 0, NULL, 0}
};

void print_setup_help(void) {
    putchar('\n');
    printf("Usage: spk setup [options]\n");
    printf("Setup spk for this system\n");
    putchar('\n');
    printf("options:\n");
    printf("    --yes, -y        Automatically answer yes for all questions\n");
    printf("    --repository, -r Use a repository instead of the default\n");
    printf("    --root, -R      Use another directory as root instead of /\n");
    printf("    --help, -h       Print this help message\n");
}

int setup(int argc, char **argv) {
    int opt;

    bool assume_yes = false;

    char *repo = NULL;
    char *root = NULL;

    while ((opt = getopt_long(argc, argv, "hyr:R:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_setup_help();
                return ERR_OK;

            case 'y':
                assume_yes = true;
                break;

            case 'r':
                repo = optarg;
                break;

            case 'R':
                root = optarg;
                break;

            case ':':
                return ERR_EXIT;

            case '?':
                return ERR_EXIT;
        }
    }

    return perform_setup(root, repo, assume_yes);
}