#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>

#include "err.h"
#include "path.h"
#include "commands/list/list.h"

int list_packages(bool no_dependencies, bool long_format) {
    std::filesystem::path package_dir(PATHDIR_INSTALLED_PKGS);
    if (!std::filesystem::exists(package_dir) || !std::filesystem::is_directory(package_dir)) {
        return ERR_EXIT;
    }

    for (const auto &entry : std::filesystem::directory_iterator(package_dir)) {
        if (entry.is_directory() && std::filesystem::exists(entry.path() / "package.metadata")) {
            if (!no_dependencies && !long_format) {
                // SKIP OPENING METADATA
                std::cout << entry.path().filename() << std::endl;
                continue;
            }

            // OPEN METADATA
            if (long_format) {
                std::ifstream metadata(entry.path() / "package.metadata");
                if (!metadata) {
                    return ERR_EXIT;
                }

                std::string version;
                std::string reason;

                std::string line;
                while (std::getline(metadata, line)) {
                    if (line.compare(0, 8, "version=") == 0) {
                        version = line.substr(8);
                    }
                    if (line.compare(0, 7, "reason=") == 0) {
                        reason = line.substr(7);
                    }
                }

                if (no_dependencies && reason == "dependency") {
                    continue;
                }

                std::cout
                    << entry.path().filename().string()
                    << " "
                    << version
                    << " "
                    << reason
                    << std::endl;

                continue;
            }

            if (no_dependencies) {
                std::ifstream metadata(entry.path() / "package.metadata");
                if (!metadata) {
                    return ERR_EXIT;
                }

                std::string line;
                while (std::getline(metadata, line)) {
                    if (line.compare(0, 7, "reason=") == 0) {
                        std::string value = line.substr(7);
                        if (value != "dependency") {
                            std::cout << entry.path().filename().string() << std::endl;
                        }
                        break;
                    }
                }
            }
        }
    }

    return ERR_OK;
}