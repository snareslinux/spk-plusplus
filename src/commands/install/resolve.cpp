#include <vector>
#include <string>
#include <algorithm>

#include "err.h"
#include "path.h"
#include "commands/install/resolve.hpp"

static bool contains_package(std::vector<InsPackage> &packages, std::string name) {
    for (const auto &package : packages) {
        if (package.name == name) {
            return true;
        }
    }

    return false;
}

static bool string_contains_package(std::vector<std::string> &packages, std::string name) {
    for (const auto &package : packages) {
        if (package == name) {
            return true;
        }
    }

    return false;
}

bool visit_package(
    std::vector<InsPackage> &plan,
    std::vector<InsPackage> &requested,
    std::vector<RepoPackage> &available,
    std::vector<std::string> &installed,
    std::vector<std::string> &visiting,
    InsPackage &package,
    bool is_dependency
) {
    if (string_contains_package(visiting, package.name)) {
        return false;
    }

    visiting.push_back(package.name);

    for (const auto &dependency : package.dependencies) {
        if (contains_package(plan, dependency)) {
            continue;
        } else if (string_contains_package(installed, dependency)) {
            continue;
        } else {
            InsPackage dep;
            if (get_package_data(available, dependency, &dep) != ERR_OK) {
                visiting.pop_back();
                return false;
            }

            if (!visit_package(plan, requested, available, installed, visiting, dep, true)) {
                visiting.pop_back();
                return false;
            }
        }
    }

    visiting.pop_back();

    package.is_dependency = is_dependency;
    plan.push_back(package);

    return true;
}

InstallPlan ins_resolve(std::vector<InsPackage> &requested, std::vector<RepoPackage> &available) {
    InstallPlan plan;

    std::vector<std::string> installed;
    if (get_installed_packages(&installed) != ERR_OK) {
        plan.error = true;
        return plan;
    }

    std::vector<std::string> visiting;

    for (auto &package : requested) {
        if (!contains_package(plan.packages, package.name) && !string_contains_package(installed, package.name)) {
            if (!visit_package(plan.packages, requested, available, installed, visiting, package, false)) {
                plan.error = true;
                return plan;
            }
        }
    }

    for (const auto &package : requested) {
        for (auto &plan_package : plan.packages) {
            if (plan_package.name == package.name) {
                plan_package.is_dependency = false;
            }
        }
    }

    return plan;
}