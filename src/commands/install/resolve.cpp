#include "commands/install/resolve.hpp"

InstallPlan ins_resolve(std::vector<InsPackage> requested) {
    InstallPlan newplan;
    
    newplan.packages = requested;

    return newplan;
}