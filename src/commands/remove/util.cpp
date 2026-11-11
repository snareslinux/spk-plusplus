#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <sstream>

#include "err.h"
#include "path.h"
#include "util/depver.hpp"
#include "commands/remove/remove.h"
#include "commands/remove/resolve.hpp"

int get_package_removaldata(std::string name, RemPackage *out) {
    if (
        name.empty() ||
        name.find_first_of("/") != std::string::npos ||
        name == "." ||
        name == ".."
    ) {
        return ERR_EXIT;
    }

    RemPackage newpkg;

    std::filesystem::path pkgdir = PATHDIR_INSTALLED_PKGS;
    pkgdir /= name;

    if (
        !std::filesystem::exists(pkgdir) ||
        !std::filesystem::is_directory(pkgdir)
    ) {
        return ERR_EXIT;
    }

    newpkg.name = name;

    std::ifstream metadata(pkgdir / "package.metadata");
    if (!metadata) {
        return ERR_EXIT;
    }

    std::string line;
        
    while (std::getline(metadata, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        if (!line.compare(0, 8, "version=")) {
            newpkg.version = line.substr(8);
        } else if (!line.compare(0, 13, "dependencies=")) {
            std::stringstream stream(line.substr(13));
            std::string dependency;

            while (std::getline(stream, dependency, ',')) {
                if (!dependency.empty()) {
                    newpkg.dependency_specs.push_back(dependency);
                    newpkg.dependencies.push_back(dependency_name(dependency));
                }
            }
        } else if (!line.compare(0, 11, "dependents=")) {
            std::stringstream stream(line.substr(11));
            std::string dependent;

            while (std::getline(stream, dependent, ',')) {
                if (!dependent.empty()) {
                    newpkg.dependents.push_back(dependent);
                }
            }
        } else if (!line.compare(0, 7, "reason=")) {
            if (line.substr(7) == "dependency") {
                newpkg.is_explicit = false;
            } else {
                newpkg.is_explicit = true;
            }
        }
    }
    

    *out = std::move(newpkg);

    return ERR_OK;
}

int remove_dependent_reference(std::string dependency, std::string dependent) {
    std::filesystem::path pkgdir = PATHDIR_INSTALLED_PKGS;
    pkgdir /= dependency;

    std::filesystem::path metadata = pkgdir / "package.metadata";
    if (!std::filesystem::exists(metadata)) {
        return ERR_OK;
    }

    std::ifstream input(metadata);
    if (!input) {
        return ERR_EXIT;
    }

    std::vector<std::string> lines;
    std::string line;
    bool found = false;
    while (std::getline(input, line)) {
        if (!line.compare(0, 11, "dependents=")) {
            found = true;

            std::stringstream stream(line.substr(11));
            std::string name;
            std::vector<std::string> dependents;

            while (std::getline(stream, name, ',')) {
                if (!name.empty() && name != dependent) {
                    dependents.push_back(name);
                }
            }

            line = "dependents=";

            for (const auto &entry : dependents) {
                line += entry + ",";
            }
        }

        lines.push_back(line);
    }

    if (input.bad()) {
        return ERR_EXIT;
    }

    if (!found) {
        return ERR_OK;
    }

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