#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdio>

#include "err.h"
#include "package/package.h"
#include "util/util.h"
#include "commands/remove/remove.h"
#include "commands/install/resolve.hpp"
#include "commands/install/install.h"
#include "ui/ui.hpp"
#include "path.h"

int perform_install(InsPackage package, std::string workdir);

int install_packages(int argc, char **argv, bool assume_yes) {
    std::vector<InsPackage> packages;
    
    for (int i = 0; i < argc; i++) {
        InsPackage tmp;
        if (get_package_data(argv[i], &tmp) == ERR_OK) {
            packages.push_back(tmp);   
        } else {
            
        }
    }

    std::filesystem::path workdir = std::filesystem::temp_directory_path() / "spkinstallation";
    std::filesystem::create_directories(workdir);

    InstallPlan plan = ins_resolve(packages);

    ui.message("Package and Dependencies:");
    for (const auto &package : plan.packages) {
        ui.message("    %s-%s", package.name.c_str(), package.version.c_str());
    }

    if (!ui.confirm(assume_yes, false, "Do you wish to install these %d packages", (int)plan.packages.size())) {
        return ERR_OK;
    }

    for (const auto &package : plan.packages) {
        if (package.local) {
            // MOVE FILE INTO DIR
            std::filesystem::copy(package.source, workdir / package.name);
        } else {
            // DOWNLOAD FILE INTO DIR
            FILE *dest = std::fopen((workdir / package.name).c_str(), "wb");
            if (!dest) {
                return ERR_EXIT;
            }
            download_to_file(package.source.c_str(), dest);
            std::fclose(dest);
        }
    }

    for (const auto &package : plan.packages) {
        switch (perform_install(package, workdir)) {
            case ERR_RETRY: {
                if (perform_install(package, workdir) != ERR_OK) {
                    return ERR_EXIT;
                }
                break;
            }
            
            case ERR_EXIT:
                return ERR_EXIT;
            
            case ERR_OK:
                break;
            default:
                break;
        }

        for (const auto &dependency : package.dependencies) {
            if (add_dependent_reference(dependency, package.name) != ERR_OK) {
                return ERR_EXIT;
            }
        }
    }

    return ERR_OK;
}

int perform_install(InsPackage package, std::string workdir) {
    std::filesystem::path spkpath(workdir);
    spkpath /= package.name;
    std::ifstream spk(spkpath, std::ios::binary);
    if (!spk) {
        return ERR_EXIT;
    }

    std::filesystem::path install_dir = PATHDIR_INSTALLED_PKGS;
    std::filesystem::path package_dir = install_dir / package.name;

    if (std::filesystem::exists(package_dir / "package.metadata")) return ERR_EXIT;

    std::filesystem::create_directories(package_dir);

        // WRITE PACKAGE INFO
    std::ofstream metadataf(package_dir / "package.metadata");
    if (!metadataf) {
        perform_remove(package_dir.c_str(), "package.files");
        return ERR_RETRY;
    }

    metadataf << "version=" << package.version << std::endl;

    metadataf << "dependencies=";
    for (const auto &dependency : package.dependencies) {
        metadataf << dependency << ",";
    }
    metadataf << std::endl;

    metadataf << "dependents=" << std::endl;

    metadataf << "reason=";
    if (package.is_dependency) {
        metadataf << "dependency";
    } else {
        metadataf << "explicit";
    }
    metadataf << std::endl;

    if (extract_spk(spkpath.c_str(), "/", (package_dir / "install.journal").c_str()) != 0) {
        // REMOVE PACKAGE
        // MAY PERFORM RETRY DEPENDING ON ERROR
        perform_remove(package_dir.c_str(), "install.journal");
        return ERR_RETRY;
    } else {
        // SET JOURNAL AS INSTALL LIST
        std::filesystem::rename(package_dir / "install.journal", package_dir / "package.files");
    }

    return ERR_OK;
}