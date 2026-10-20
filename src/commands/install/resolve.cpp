#include "commands/install/resolve.hpp"

InstallPlan resolve(std::vector<Package> requested) {
    InstallPlan newplan;
    for (int i = 0; i < requested.size(); i++) {
        newplan.packages.push_back(requested[i]);
    }
    return newplan;
}