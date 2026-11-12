#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include "path.h"
#include "err.h"
#include "ui/ui.hpp"
#include "commands/remove/remove.h"
#include "commands/remove/resolve.hpp"

int remove_packages(int argc, char **argv, bool assume_yes, bool force, const char *root) {
    std::string target_root = root ? root : "/";
    if (target_root.back() != '/') {
        target_root.push_back('/');
    }

    std::vector<RemPackage> packages;
    for (int i = 0; i < argc; i++) {
        RemPackage tmp;
        if (get_package_removaldata(argv[i], target_root, &tmp) != ERR_OK) {
            return ERR_EXIT;
        }
        packages.push_back(tmp);
    }

    RemovePlan plan = {};
    if (force) {
        plan.error = false;
        plan.packages = packages;
    } else {
        plan = rem_resolve(packages, target_root);
    }

    if (plan.error) {
        ui.message("Could not resolve the removal plan.");
        return ERR_EXIT;
    }

    if (!plan.blocked.empty()) {
        ui.message("Cannot remove these packages because other installed packages depend on them:");

        for (const auto &name : plan.blocked) {
            ui.message("    %s", name.c_str());
        }

        return ERR_EXIT;
    }

    if (plan.packages.empty()) {
        ui.message("No packages to remove.");
        return ERR_OK;
    }

    ui.message("Package that will be removed:");
    for (const auto &package : plan.packages) {
        ui.message("    %s-%s", package.name.c_str(), package.version.c_str());
    }

    if (!ui.confirm(assume_yes, false, "Do you wish to remove these %d packages", (int)plan.packages.size())) {
        ui.message("Operation cancelled by user.");
        return ERR_OK;
    }

    for (const auto &package : plan.packages) {
        std::filesystem::path pkgdir = target_root;
        pkgdir /= PATHDIR_INSTALLED_PKGS;
        pkgdir /= package.name;

        if (perform_remove(pkgdir.c_str(), "package.files", target_root.c_str()) != ERR_OK) {
            ui.message("Failed to remove package: %s", package.name.c_str());
            return ERR_EXIT;
        }

        for (const auto &dependency : package.dependencies) {
            if (remove_dependent_reference(dependency, package.name, target_root) != ERR_OK) {
                return ERR_EXIT;
            }
        }
    }

    return ERR_OK;
}

int perform_remove(const char *pkgdir, const char *target, const char *root) {
    std::filesystem::path package_dir(pkgdir);
    std::ifstream files(package_dir / target);

    if (!files) {
        return ERR_EXIT;
    }

    std::vector<std::string> used_dirs;

    std::string line;
    while (std::getline(files, line)) {
        if (line.empty()) {
            continue;
        }

        if (line.size() < 3 || line[1] != ':') {
            return ERR_EXIT;
        }

        std::string path = line.substr(2);

        if (path.empty()) {
            return ERR_EXIT;
        }

        std::filesystem::path filepath(path);
        std::error_code ec;

        if (line[0] == 'd') {
            // DIRECTORY
            if (!std::filesystem::exists(filepath, ec)) {
                if (ec) {
                    return ERR_EXIT;
                }

                continue;
            }

            if (!std::filesystem::is_directory(filepath, ec)) {
                return ERR_EXIT;
            }

            used_dirs.push_back(path);
        } else if (line[0] == 'l') {
            // SYMLINK
            bool is_symlink = std::filesystem::is_symlink(filepath, ec);
            if (ec) {
                return ERR_EXIT;
            }

            if (is_symlink) {
                std::filesystem::remove(filepath, ec);

                if (ec) {
                    return ERR_EXIT;
                }
            }
        } else if (line[0] == 'f') {
            // FILE
            std::filesystem::remove(filepath, ec);

            if (ec) {
                return ERR_EXIT;
            }
        } else {
            return ERR_EXIT;
        }
    }

    if (files.bad()) {
        return ERR_EXIT;
    }

    std::sort(used_dirs.begin(), used_dirs.end(), [](const auto &a, const auto &b) {
        return a.size() > b.size();
    });

    std::filesystem::path unowned_dir = std::filesystem::path(root) / PATHDIR_INSTALLED_PKGS / "unowned";
    std::error_code ec;

    std::filesystem::create_directories(unowned_dir, ec);
    if (ec) {
        return ERR_EXIT;
    }

    std::ofstream unowned(unowned_dir / "unowned.files", std::ios::app);
    if (!unowned) {
        return ERR_EXIT;
    }

    // REMOVE DIRS OR ADD TO UNOWNED
    for (const auto &dir : used_dirs) {
        std::filesystem::path path(dir);

        if (!std::filesystem::exists(path, ec)) {
            if (ec) {
                return ERR_EXIT;
            }

            continue;
        }
        
        if (!std::filesystem::is_empty(path, ec)) {
            // ADD DIRECTORY TO GLOBAL UNONWED FILES
            if (ec) {
                return ERR_EXIT;
            }

            unowned << dir << std::endl;

            if (!unowned) {
                return ERR_EXIT;
            }

            continue;
        }

        std::filesystem::remove(path, ec);

        if (ec) {
            return ERR_EXIT;
        }
    }

    unowned.close();

    if (!unowned) {
        return ERR_EXIT;
    }

    // REMOVE PACKAGE DATA
    std::filesystem::remove_all(package_dir, ec);
    if (ec) {
        return ERR_EXIT;
    }

    return ERR_OK;
}