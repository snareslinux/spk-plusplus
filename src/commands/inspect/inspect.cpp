#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "err.h"
#include "path.h"
#include "commands/commands.h"

int inspect(const char *package) {
    std::filesystem::path package_dir(PATHDIR_INSTALLED_PKGS);
    package_dir /= package;

    if (
        !std::filesystem::exists(package_dir) ||
        !std::filesystem::is_directory(package_dir) ||
        !std::filesystem::exists(package_dir / "package.files")
    ) {
        return ERR_EXIT;
    }

    std::ifstream files(package_dir / "package.files");
    if (!files) {
        return ERR_EXIT;
    }

    std::string line;
    while (std::getline(files, line)) {
        if (line.length() <= 2) {
            continue;
        }
        
        std::cout << line.substr(2) << std::endl; 
    }

    return ERR_OK;
}