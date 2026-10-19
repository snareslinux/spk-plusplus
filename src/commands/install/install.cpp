#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>

#include "package/package.h"
#include "path.h"
#include "install.h"

int perform_install(const char *spkpath) {
    std::ifstream spk(spkpath, std::ios::binary);
    if (!spk) {

    }

    struct metadata metadata;
    get_metadata(spkpath, &metadata);

    std::filesystem::path install_dir = PATHDIR_INSTALLED_PKGS;
    std::filesystem::path package_dir = install_dir / metadata.name;
    std::filesystem::create_directories(package_dir);

    if (extract_spk(spkpath, "/", (package_dir / "install.journal").c_str()) != 0) {
        // REMOVE PACKAGE
        // MAY PERFORM RETRY DEPENDING ON ERROR
    } else {
        // SET JOURNAL AS INSTALL LIST
    }

    return 0;
}