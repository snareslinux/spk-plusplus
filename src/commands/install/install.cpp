#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>

#include "err.h"
#include "package/package.h"
#include "commands/remove/remove.h"
#include "path.h"
#include "install.h"

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
    std::ofstream metadataf(package_dir / "package.meta");
    if (!metadataf) {
        perform_remove(package_dir.c_str(), "package.metadata");
        return ERR_RETRY;
    }

    return ERR_OK;
}