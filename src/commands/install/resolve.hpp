#include <string>
#include <vector>

#ifndef INSTALL_RESOLVE_HPP
#define INSTALL_RESOLVE_HPP

struct InsPackage {
    std::string name;
    std::string version;
    std::vector<std::string> dependencies;
    std::vector<std::string> dependents;
    std::string source;
    bool local;
    bool is_dependency;
};

int get_package_data(std::string name, InsPackage *out);

struct InstallPlan {
    std::vector<InsPackage> packages;
};

InstallPlan ins_resolve(std::vector<InsPackage> requested);

#endif