#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cstring>

#include "err.h"
#include "path.h"
#include "ui/ui.hpp"
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

int add_dependent_reference(std::string dependency, std::string dependent) {
    std::filesystem::path pkgdir = PATHDIR_INSTALLED_PKGS;
    pkgdir /= dependency;
    
    std::filesystem::path metadata = pkgdir / "package.metadata";
    if (!std::filesystem::exists(metadata)) {
        return ERR_EXIT;
    }

    std::ifstream input(metadata);
    if (!input) {
        return ERR_EXIT;
    }

    std::vector<std::string> lines;
    std::string line;
    std::vector<std::string> dependents;

    while (std::getline(input, line)) {
        if (!line.compare(0, 11, "dependents=")) {
            std::stringstream stream(line.substr(11));

            while (std::getline(stream, line, ',')) {
                if (!line.empty()) {
                    dependents.push_back(line);
                }
            }
        } else {
            lines.push_back(line);
        }
    }

    if (input.bad()) {
        return ERR_EXIT;
    }

    if (std::find(dependents.begin(), dependents.end(), dependent) == dependents.end()) {
        dependents.push_back(dependent);
    }

    std::string newline = "dependents=";

    for (const auto &name : dependents) {
        newline += name + ",";
    }

    lines.push_back(newline);

    std::filesystem::path temp = metadata;
    temp += ".tmp";
    
    std::ofstream output(temp);
    if (!output) {
        return ERR_EXIT;
    }

    for (const auto &entry : lines) {
        output << entry << std::endl;
    }

    output.close();
    if (!output) {
        std::filesystem::remove(temp);
        return ERR_EXIT;
    }

    std::error_code ec;
    std::filesystem::rename(temp, metadata, ec);
    if (ec) {
        std::filesystem::remove(temp);
        return ERR_EXIT;
    }

    return ERR_OK;
}