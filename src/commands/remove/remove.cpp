#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "path.h"
#include "err.h"

int perform_remove(const char *pkgdir, const char *target) {
    std::filesystem::path packagedir(pkgdir);
    std::ifstream files(packagedir / target);

    std::vector<std::string> used_dirs;

    std::string line;
    while (!files.eof()) {
        std::getline(files, line, '\n');
        if (line[0] == 'd') {
            // DIRECTORY
            if (std::filesystem::is_empty(line.substr(1))) {
                std::filesystem::remove(line.substr(1));
            } else {
                // REMOVE ALL FILES AND CHECK AT THE END IF EMPTY
                used_dirs.push_back(line.substr(1));
            }
        } else if (line[0] == 'l') {
            // SYMLINK
        } else {
            // FILE
            std::filesystem::remove(line.substr(1));
        }
    }

    std::filesystem::create_directories(std::filesystem::path(PATHDIR_INSTALLED_PKGS) / "unowned");
    std::ofstream unowned(std::filesystem::path(PATHDIR_INSTALLED_PKGS) / "unowned" / "unowned.files", std::ios::app);
    if (!unowned) {
        return ERR_EXIT;
    }

    for (int i = 0; i < used_dirs.size(); i++) {
        if (std::filesystem::is_empty(used_dirs[i])) {
            std::filesystem::remove(used_dirs[i]);
        } else {
            // ADD DIRECTORY TO GLOBAL UNONWED FILES
            unowned << used_dirs[i];
        }
    }

    return ERR_OK;
}