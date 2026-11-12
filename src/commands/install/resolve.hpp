#ifndef INSTALL_RESOLVE_HPP
#define INSTALL_RESOLVE_HPP

#include <string>
#include <vector>
#include "repository/repository.hpp"
#include "commands/remove/resolve.hpp"
#include "util/depver.hpp"

struct InsPackage {
    std::string name;
    std::string version;
    std::vector<std::string> dependencies;
    std::vector<std::string> dependents;
    std::string source;
    bool local = false;
    bool is_dependency = false;
    bool upgrade = false;
    std::string old_version;
    std::string sha256;
};

int get_package_data(std::vector<RepoPackage> &available, std::string name, InsPackage *out, const Dependency *constraint = nullptr);
int add_dependent_reference(std::string dependency, std::string dependent, std::string root);
int get_installed_packages(std::vector<RemPackage> *out, std::string root);

struct InstallPlan {
    std::vector<InsPackage> packages;
    bool error = false;
};

InstallPlan ins_resolve(std::vector<InsPackage> &requested, std::vector<RepoPackage> &available, bool mark_explicit, std::string root);
int apply_install(std::vector<InsPackage> &requested, std::vector<RepoPackage> &available, bool assume_yes, bool mark_explicit, std::string root);

#endif