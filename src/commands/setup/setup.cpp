#include <string>
#include <filesystem>
#include <sys/stat.h>
#include <cstdio>

#include "err.h"
#include "path.h"
#include "ui/ui.hpp"
#include "util/util.h"
#include "commands/setup/setup.h"

int perform_setup(const char *root, const char *repo, bool assume_yes) {
    std::string str_root = root ? root : "/";
    if (str_root.back() != '/') {
        str_root.push_back('/');
    }
    std::filesystem::path target_root = str_root;

    if (!ui.confirm(assume_yes, false, "Ensure existing configurations for spk will be overwritten. Would you still like to continue?")) {
        ui_message("Operation cancelled by user");
    }

    std::error_code ec;

    std::filesystem::create_directories((target_root / "etc" / "spk"), ec);
    if (ec) {
        return ERR_EXIT;
    }

    std::filesystem::path spk_conf_path = target_root / "etc" / "spk" / "spk.conf";

    FILE *spkconf = std::fopen(spk_conf_path.c_str(), "wb");
    if (spkconf) {
        chmod(spk_conf_path.c_str(), 0644);
        if (download_to_file(URL_CONFIG, spkconf) != 0) {
            std::fclose(spkconf);
            return ERR_EXIT;
        }

        std::fclose(spkconf);
    }

    std::filesystem::path repo_conf_path = target_root / "etc" / "spk" / "repo.conf";

    FILE *repoconf = fopen(repo_conf_path.c_str(), "wb");
    if (repoconf) {
        chmod(repo_conf_path.c_str(), 0644);

        if (!repo) {
            if (download_to_file(URL_REPOS, repoconf) != 0) {
                std::fclose(repoconf);
                return ERR_EXIT;
            }
        } else {
            std::fprintf(repoconf, "main %s\n", repo);
        }

        std::fclose(repoconf);
    }

    std::filesystem::create_directories((target_root / PATHDIR_INSTALLED_PKGS), ec);
    if (ec) {
        return ERR_EXIT;
    }

    return ERR_OK;
}