#include <fstream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <string>

#include "path.h"
#include "err.h"
#include "commands/remove/resolve.hpp"

static bool contains_name(const std::vector<std::string> &names, const std::string &name) {
    return std::find(names.begin(), names.end(), name) != names.end();
}

static bool has_dependency(const RemPackage &package, const std::string &name) {
    return contains_name(package.dependencies, name);
}

static RemPackage *find_package(std::vector<RemPackage> &packages, const std::string &name) {
    for (auto &package : packages) {
        if (package.name == name) {
            return &package;
        }
    }

    return nullptr;
}

RemovePlan rem_resolve(std::vector<RemPackage> requested) {
    RemovePlan plan;
    std::vector<RemPackage> installed;

    std::filesystem::path pkgdir = PATHDIR_INSTALLED_PKGS;

    std::error_code ec;

    for (const auto &entry : std::filesystem::directory_iterator(pkgdir, ec)) {
        if (ec) {
            plan.error = true;
            return plan;
        }

        if (!entry.is_directory(ec)) {
            if (ec) {
                plan.error = true;
                return plan;
            }

            continue;
        }

        if (!std::filesystem::exists(entry.path() / "package.metadata", ec)) {
            if (ec) {
                plan.error = true;
                return plan;
            }

            continue;
        }

        RemPackage package;

        if (get_package_removaldata(entry.path().filename().string(), &package) != ERR_OK) {
            plan.error = true;
            return plan;
        }

        installed.push_back(package);
    }

    if (ec) {
        plan.error = true;
        return plan;
    }

    std::vector<std::string> removal;

    for (const auto &package : requested) {
        if (!find_package(installed, package.name)) {
            plan.error = true;
            return plan;
        }

        if (!contains_name(removal, package.name)) {
            removal.push_back(package.name);
        }
    }

    for (size_t i = 0; i < removal.size(); i++) {
        RemPackage *package = find_package(installed, removal[i]);

        if (!package) {
            plan.error = true;
            return plan;
        }

        for (const auto &dependency : package->dependencies) {
            RemPackage *dep = find_package(installed, dependency);

            if (!dep || dep->is_explicit) {
                continue;
            }

            if (!contains_name(removal, dependency)) {
                removal.push_back(dependency);
            }
        }
    }

    bool changed = true;

    while (changed) {
        changed = false;

        for (size_t i = 0; i < removal.size();) {
            const std::string name = removal[i];

            bool explicitly_requested = false;

            for (const auto &package : requested) {
                if (package.name == name) {
                    explicitly_requested = true;
                    break;
                }
            }

            if (explicitly_requested) {
                i++;
                continue;
            }

            bool needed = false;

            for (const auto &package : installed) {
                if (has_dependency(package, name) && !contains_name(removal, package.name)) {
                    needed = true;
                    break;
                }
            }

            if (needed) {
                removal.erase(removal.begin() + i);
                changed = true;
            } else {
                i++;
            }
        }
    }

    for (const auto &package : requested) {
        for (const auto &other : installed) {
            if (has_dependency(other, package.name) && !contains_name(removal, other.name)) {
                plan.blocked.push_back(package.name);
                break;
            }
        }
    }

    if (!plan.blocked.empty()) {
        return plan;
    }

    std::vector<std::string> remaining = removal;

    while (!remaining.empty()) {
        bool found = false;

        for (size_t i = 0; i < remaining.size(); i++) {
            const std::string name = remaining[i];
            bool has_remaining_dependent = false;

            for (const auto &other_name : remaining) {
                if (other_name == name) {
                    continue;
                }

                RemPackage *other = find_package(installed, other_name);

                if (other && has_dependency(*other, name)) {
                    has_remaining_dependent = true;
                    break;
                }
            }

            if (!has_remaining_dependent) {
                RemPackage *package = find_package(installed, name);

                if (!package) {
                    plan.error = true;
                    return plan;
                }

                plan.packages.push_back(*package);
                remaining.erase(remaining.begin() + i);
                found = true;
                break;
            }
        }

        if (!found) {
            RemPackage *package = find_package(installed, remaining.front());

            if (!package) {
                plan.error = true;
                return plan;
            }

            plan.packages.push_back(*package);
            remaining.erase(remaining.begin());
        }
    }

    return plan;
}