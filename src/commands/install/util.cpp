#include <string>
#include <filesystem>
#include <sstream>
#include <cstring>

#include "err.h"
#include "package/package.h"
#include "commands/install/install.h"
#include "commands/install/resolve.hpp"

int get_package_data(std::string name, InsPackage *out) {
    if (name.find_first_of("/") != std::string::npos) {
        return ERR_EXIT;
    }
    
    InsPackage newpkg;

    if (std::filesystem::exists(name)) {
        // LOCAL FILE
        struct metadata metadata;
        std::memset(&metadata, 0, sizeof(metadata));
        if (get_metadata(name.c_str(), &metadata) != ERR_OK) {
            return ERR_EXIT;
        }
        newpkg.local = true;
        newpkg.is_dependency = false;
        newpkg.name = metadata.name;
        newpkg.version = metadata.version;
        newpkg.source = name;
        std::stringstream depstream(metadata.dependencies);
        std::string pkg;
        while (std::getline(depstream, pkg, ',')) {
            newpkg.dependencies.push_back(pkg);
        }
    } else {
        // SEARCH REPO
    }

    *out = std::move(newpkg);

    return ERR_OK;
}