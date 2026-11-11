#include <vector>
#include <string>
#include <algorithm>

#include "err.h"
#include "util/depver.hpp"
#include "commands/install/resolve.hpp"
#include "commands/remove/resolve.hpp"

static InsPackage *find_planned(std::vector<InsPackage> &plan, const std::string &name) {
    for (auto &package : plan) {
        if (package.name == name) {
            return &package;
        }
    }

    return nullptr;
}

static RemPackage *find_installed(std::vector<RemPackage> &installed, const std::string &name) {
    for (auto &package : installed) {
        if (package.name == name) {
            return &package;
        }
    }

    return nullptr;
}

static bool string_contains_package(std::vector<std::string> &packages, std::string name) {
    for (const auto &package : packages) {
        if (package == name) {
            return true;
        }
    }

    return false;
}

static bool get_final_version(
    std::vector<InsPackage> &plan,
    std::vector<RemPackage> &installed,
    const std::string &name,
    std::string *out
) {
    InsPackage *planned = find_planned(plan, name);
    if (planned) {
        *out = planned->version;
        return true;
    }

    RemPackage *package = find_installed(installed, name);
    if (package) {
        *out = package->version;
        return true;
    }

    return false;
}

bool visit_package(
    std::vector<InsPackage> &plan,
    std::vector<RepoPackage> &available,
    std::vector<RemPackage> &installed,
    std::vector<std::string> &visiting,
    InsPackage &package,
    bool is_dependency
) {
    if (string_contains_package(visiting, package.name)) {
        return false;
    }

    visiting.push_back(package.name);

    for (const auto &spec : package.dependencies) {
        Dependency dependency;
        if (parse_dependency(spec, &dependency) != ERR_OK) {
            visiting.pop_back();
            return false;
        }

        std::string version;
        if (get_final_version(plan, installed, dependency.name, &version)) {
            if (version_satisfies(version, dependency)) {
                continue;
            }
        }

        if (find_planned(plan, dependency.name)) {
            visiting.pop_back();
            return false;
        }
        
        InsPackage dep;
        if (get_package_data(available, dependency.name, &dep, &dependency) != ERR_OK) {
            visiting.pop_back();
            return false;
        }

        if (!visit_package(plan, available, installed, visiting, dep, true)) {
            visiting.pop_back();
            return false;
        }
    }

    visiting.pop_back();

    RemPackage *old = find_installed(installed, package.name);
    if (old) {
        package.upgrade = true;
        package.old_version = old->version;
        package.is_dependency = !old->is_explicit;
    } else {
        package.is_dependency = is_dependency;
    }

    plan.push_back(package);

    return true;
}

static bool check_dependencies(
    const std::vector<std::string> &specs,
    std::vector<InsPackage> &plan,
    std::vector<RemPackage> &installed,
    bool only_changed
) {
    for (const auto &spec : specs) {
        Dependency dependency;
        if (parse_dependency(spec, &dependency) != ERR_OK) {
            return false;
        }

        if (only_changed && !find_planned(plan, dependency.name)) {
            continue;
        }

        std::string version;
        if (!get_final_version(plan, installed, dependency.name, &version)) {
            return false;
        }

        if (!version_satisfies(version, dependency)) {
            return false;
        }
    }

    return true;
}

static bool validate_plan(std::vector<InsPackage> &plan, std::vector<RemPackage> &installed) {
    for (const auto &package : plan) {
        if (!check_dependencies(package.dependencies, plan, installed, false)) {
            return false;
        }
    }

    for (const auto &package : installed) {
        if (find_planned(plan, package.name)) {
            continue;
        }

        if (!check_dependencies(package.dependency_specs, plan, installed, true)) {
            return false;
        }
    }

    return true;
}

InstallPlan ins_resolve(std::vector<InsPackage> &requested, std::vector<RepoPackage> &available, bool mark_explicit) {
    InstallPlan plan;

    std::vector<RemPackage> installed;
    if (get_installed_packages(&installed) != ERR_OK) {
        plan.error = true;
        return plan;
    }

    std::vector<std::string> visiting;

    for (auto &package : requested) {
        if (find_planned(plan.packages, package.name)) {
            continue;
        }

        RemPackage *old = find_installed(installed, package.name);
        if (old && compare_versions(package.version, old->version) <= 0) {
            continue;
        }

        if (!visit_package(plan.packages, available, installed, visiting, package, false)) {
            plan.error = true;
            return plan;
        }
    }

    if (!validate_plan(plan.packages, installed)) {
        plan.error = true;
        return plan;
    }

    if (mark_explicit) {
        for (const auto &package : requested) {
            InsPackage *planned = find_planned(plan.packages, package.name);
            if (planned) {
                planned->is_dependency = false;
            }
        }
    }

    return plan;
}