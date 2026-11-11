#include <vector>

#include "err.h"
#include "util/depver.hpp"
#include "repository/repository.hpp"
#include "commands/install/resolve.hpp"
#include "commands/upgrade/upgrade.h"

int upgrade_packages(bool assume_yes) {
    std::vector<RepoPackage> available;
    if (get_available_packages(&available) != ERR_OK) {
        return ERR_EXIT;
    }

    std::vector<RemPackage> installed;
    if (get_installed_packages(&installed) != ERR_OK) {
        return ERR_EXIT;
    }

    std::vector<InsPackage> upgrades;

    for (const auto &package : installed) {
        RepoPackage best;
        if (find_package(available, package.name, &best) != ERR_OK) {
            continue;
        }

        if (compare_versions(best.version, package.version) <= 0) {
            continue;
        }

        InsPackage tmp;
        if (get_package_data(available, package.name, &tmp) != ERR_OK) {
            return ERR_EXIT;
        }

        upgrades.push_back(tmp);
    }

    if (upgrades.empty()) {
        return ERR_OK;
    }

    return apply_install(upgrades, available, assume_yes, false);
}