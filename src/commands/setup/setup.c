#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "util/util.h"
#include "path.h"
#include "commands/commands.h"

int setup(void) {
    if (mkdir_p("/etc/spk", 0755) != 0) {
        return 1;
    }

    FILE *spkconf = fopen(PATH_SPK_CONF, "wb");
    if (spkconf) {
        chmod(PATH_SPK_CONF, 0644);
        if (download_to_file(URL_CONFIG, spkconf) != 0) {
            fclose(spkconf);
            return 1;
        }

        fclose(spkconf);
    }

    FILE *repoconf = fopen(PATH_REPO_CONF, "wb");
    if (repoconf) {
        chmod(PATH_REPO_CONF, 0644);
        if (download_to_file(URL_REPOS, repoconf) != 0) {
            fclose(repoconf);
            return 1;
        }

        fclose(repoconf);
    }

    if (mkdir_p(PATHDIR_INSTALLED_PKGS, 0755) != 0) {
        return 1;
    }

    return 0;
}