#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <format>

#include "err.h"
#include "path.h"
#include "ui/color.hpp"
#include "commands/info/info.h"
#include "repository/repository.hpp"

int print_package_info(const char *package, bool online, bool installed) {
    if (!online && !installed) {
        online = true;
        installed = true;
    }

    if (installed) {
        std::cout << BOLD + "INSTALLED" + RESET << std::endl << std::endl;

        std::filesystem::path package_dir(PATHDIR_INSTALLED_PKGS);
        if (!std::filesystem::exists(package_dir) || !std::filesystem::is_directory(package_dir)) {
            return ERR_EXIT;
        }

        for (const auto &entry : std::filesystem::directory_iterator(package_dir)) {
            if (
                entry.path().filename().string() == package &&
                entry.is_directory() &&
                std::filesystem::exists(entry.path() / "package.metadata")
            ) { 
                std::ifstream metadata(entry.path() / "package.metadata");
                if (!metadata) {
                    return ERR_EXIT;
                }

                std::string version;
                std::string reason;
                std::string dependencies;
                std::string dependents;

                std::string line;
                while (std::getline(metadata, line)) {
                    if (line.compare(0, 8, "version=") == 0) {
                        version = line.substr(8);
                    }
                    if (line.compare(0, 7, "reason=") == 0) {
                        reason = line.substr(7);
                    }
                    if (line.compare(0, 13, "dependencies=") == 0) {
                        dependencies = line.substr(13);
                    }
                    if (line.compare(0, 11, "dependents=") == 0) {
                        dependents = line.substr(11);
                    }
                }

                std::cout << entry.path().filename().string() << std::endl;
                std::cout << "    version " << version << std::endl;
                std::cout << "    reason " << reason << std::endl;
                std::cout << "    dependencies " << dependencies << std::endl;
                std::cout << "    dependents " << dependents << std::endl;
                std::cout << std::endl;
            }
        }
    }

    if (online) {
        std::cout << BOLD + "ONLINE" + RESET <<  std::endl << std::endl;

        std::vector<RepoPackage> available;
        if (get_available_packages(&available) != ERR_OK) {
            return ERR_EXIT;
        }

        for (const auto &entry : available) {
            if (entry.name == package) {
                std::cout << entry.name << std::endl;
                std::cout << std::format("    version {}", entry.version) << std::endl;
                std::cout << std::format("    architecture {}", entry.arch) << std::endl;
                std::cout << std::format("    repository {}", entry.repository_name) << std::endl;
                std::cout << std::endl;
            }
        }
    }

    return ERR_OK;
}