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

static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"repository", required_argument, NULL, 'r'},
    {"yes", required_argument, NULL, 'y'},
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
    printf("    --help, -h       Print this help message\n");
}

int setup(int argc, char **argv) {
    int opt;

    bool default_repo = true;
    bool assume_yes = false;

    char *repo = 0;

    while ((opt = getopt_long(argc, argv, "hyr:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_setup_help();
                return ERR_OK;

            case 'y':
                assume_yes = true;
                break;

            case 'r':
                default_repo = false;
                repo = optarg;
                break;

            case ':':
                return ERR_EXIT;
        }
    }

    if (mkdir_p("/etc/spk", 0755) != 0) {
        return ERR_EXIT;
    }

    if (!ui_confirm(assume_yes, false, "Ensure existing configurations for spk will be overwritten. Would you still like to continue?")) {
        ui_message("Operation cancelled by user");
    }

    FILE *spkconf = fopen(PATH_SPK_CONF, "wb");
    if (spkconf) {
        chmod(PATH_SPK_CONF, 0644);
        if (download_to_file(URL_CONFIG, spkconf) != 0) {
            fclose(spkconf);
            return ERR_EXIT;
        }

        fclose(spkconf);
    }

    FILE *repoconf = fopen(PATH_REPO_CONF, "wb");
    if (repoconf) {
        chmod(PATH_REPO_CONF, 0644);

        if (default_repo) {
            if (download_to_file(URL_REPOS, repoconf) != 0) {
                fclose(repoconf);
                return ERR_EXIT;
            }
        } else {
            fprintf(repoconf, "main %s\n", repo);
        }

        fclose(repoconf);
    }

    if (mkdir_p(PATHDIR_INSTALLED_PKGS, 0755) != 0) {
        return ERR_EXIT;
    }

    return ERR_OK;
}