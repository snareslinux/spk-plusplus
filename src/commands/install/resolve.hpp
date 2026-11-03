#ifndef INSTALL_RESOLVE_HPP
#define INSTALL_RESOLVE_HPP

#include <string>
#include <vector>
#include "repository/repository.hpp"

struct InsPackage {
    std::string name;
    std::string version;
    std::vector<std::string> dependencies;
    std::vector<std::string> dependents;
    std::string source;
    bool local = false;
    bool is_dependency = false;
    std::string sha256;
};

int get_package_data(std::vector<RepoPackage> &available, std::string name, InsPackage *out);
int add_dependent_reference(std::string dependency, std::string dependent);
int get_installed_packages(std::vector<std::string> *out);

struct InstallPlan {
    std::vector<InsPackage> packages;
    bool error = false;
};

InstallPlan ins_resolve(std::vector<InsPackage> &requested, std::vector<RepoPackage> &available);

#endif