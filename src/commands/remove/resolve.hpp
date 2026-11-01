#ifndef REMOVE_RESOLVE_HPP
#define REMOVE_RESOLVE_HPP

#include <vector>
#include <string>

struct RemPackage {
    std::string name;
    std::string version;
    std::vector<std::string> dependencies;
    std::vector<std::string> dependents;
    bool is_explicit = true;
};

int get_package_removaldata(std::string name, RemPackage *out);
int remove_dependent_reference(std::string dependency, std::string dependent);

struct RemovePlan {
    std::vector<RemPackage> packages;
    std::vector<std::string> blocked;
    bool error = false;
};

RemovePlan rem_resolve(std::vector<RemPackage> requested);

#endif