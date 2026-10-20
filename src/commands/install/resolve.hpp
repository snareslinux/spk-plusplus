#include <string>
#include <vector>

#ifndef RESOLVE_HPP
#define RESOLVE_HPP

struct Package {
    std::string name;
    std::string version;
    std::vector<std::string> dependencies;
    std::string source;
    bool local;
};

int get_package_data(std::string name, Package *out);

struct InstallPlan {
    std::vector<Package> packages;
};

InstallPlan resolve(std::vector<Package> requested);

#endif