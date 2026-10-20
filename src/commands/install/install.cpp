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

int install_packages(int argc, char **argv, bool assume_yes) {
    std::vector<Package> packages;
    
    for (int i = 0; i < argc; i++) {
        Package tmp;
        if (get_package_data(argv[i], &tmp) == ERR_OK) {
            packages.push_back(tmp);   
        } else {
            
        }
    }

    std::filesystem::path workdir = std::filesystem::temp_directory_path() / "spkinstallation";
    std::filesystem::create_directories(workdir);

    InstallPlan plan = resolve(packages);

    ui.message("Package and Dependencies:");
    for (int i = 0; i < plan.packages.size(); i++) {
        ui.message("    %s-%s", plan.packages[i].name.c_str(), plan.packages[i].version.c_str());
    }

    if (!ui.confirm(assume_yes, false, "Do you wish to install these %d packages", (int)plan.packages.size())) {
        return ERR_OK;
    }

    for (int i = 0; i < plan.packages.size(); i++) {
        if (plan.packages[i].local) {
            // MOVE FILE INTO DIR
            std::filesystem::copy(plan.packages[i].source, workdir / plan.packages[i].name);
        } else {
            // DOWNLOAD FILE INTO DIR
            FILE *dest = fopen((workdir / plan.packages[i].name).c_str(), "wb");
            if (!dest) {
                return ERR_EXIT;
            }
            download_to_file(plan.packages[i].source.c_str(), dest);
        }
    }

    for (int i = 0; i < plan.packages.size(); i++) {
        switch (perform_install((workdir / plan.packages[i].name).c_str())) {
            case ERR_RETRY: {
                if (perform_install((workdir / plan.packages[i].name).c_str()) != ERR_OK) {
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
    }

    return ERR_OK;
}

int perform_install(const char *spkpath) {
    std::ifstream spk(spkpath, std::ios::binary);
    if (!spk) {
        return ERR_EXIT;
    }

    struct metadata metadata;
    get_metadata(spkpath, &metadata);

    std::filesystem::path install_dir = PATHDIR_INSTALLED_PKGS;
    std::filesystem::path package_dir = install_dir / metadata.name;
    std::filesystem::create_directories(package_dir);

    if (extract_spk(spkpath, "/", (package_dir / "install.journal").c_str()) != 0) {
        // REMOVE PACKAGE
        // MAY PERFORM RETRY DEPENDING ON ERROR
        perform_remove(package_dir.c_str(), "install.journal");
        return ERR_RETRY;
    } else {
        // SET JOURNAL AS INSTALL LIST
        std::filesystem::rename(package_dir / "install.journal", package_dir / "package.files");
    }

    // WRITE PACKAGE INFO
    std::ofstream metadataf(package_dir / "package.metadata");
    if (!metadataf) {
        perform_remove(package_dir.c_str(), "package.metadata");
        return ERR_RETRY;
    }

    return ERR_OK;
}