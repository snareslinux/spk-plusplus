#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdio> 

#include "err.h"
#include "path.h"
#include "ui/ui.hpp"
#include "package/package.h"
#include "util/util.h"
#include "util/util.hpp"
#include "util/depver.hpp"
#include "repository/repository.hpp"
#include "commands/remove/remove.h"
#include "commands/remove/resolve.hpp"
#include "commands/install/resolve.hpp"
#include "commands/install/install.h"

int perform_install(InsPackage package, std::string workdir, std::string root);
int perform_upgrade(InsPackage &package, std::string workdir, std::string root);

int install_packages(int argc, char **argv, bool assume_yes, const char *root) {
    std::string target_root = root ? root : "/";
    if (target_root.back() != '/') {
        target_root.push_back('/');
    }

    std::vector<InsPackage> packages;
    std::vector<RepoPackage> available;
    if (get_available_packages(&available) != ERR_OK) {
        return ERR_EXIT;
    }
    
    for (int i = 0; i < argc; i++) {
        InsPackage tmp;
        if (get_package_data(available, argv[i], &tmp) != ERR_OK) {
            return ERR_EXIT;     
        }

        packages.push_back(tmp); 
    }

    if (packages.empty()) {
        return ERR_EXIT;
    }

    return apply_install(packages, available, assume_yes, true, target_root);
}

int apply_install(std::vector<InsPackage> &requested, std::vector<RepoPackage> &available, bool assume_yes, bool mark_explicit, std::string root) {
    std::filesystem::path workdir = std::filesystem::temp_directory_path() / "spkinstallation";
    std::filesystem::create_directories(workdir);

    for (const auto &entry : std::filesystem::directory_iterator(workdir)) {
        std::error_code ec;
        std::filesystem::remove_all(entry.path(), ec);
        if (ec) {
            return ERR_EXIT;
        }
    }

    InstallPlan plan = ins_resolve(requested, available, mark_explicit, root);
    if (plan.error) {
        return ERR_EXIT;
    }

    if (plan.packages.empty()) {
        return ERR_OK;
    }

    ui.message("Package and Dependencies:");
    for (const auto &package : plan.packages) {
        if (package.upgrade) {
            ui.message("    %s %s -> %s", package.name.c_str(), package.old_version.c_str(), package.version.c_str());
        } else {
            ui.message("    %s-%s", package.name.c_str(), package.version.c_str());
        }
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
            std::filesystem::path dest_path = workdir / package.name;
            FILE *dest = std::fopen(dest_path.c_str(), "wb");
            if (!dest) {
                return ERR_EXIT;
            }
            if (download_to_file(package.source.c_str(), dest) != ERR_OK) {
                std::fclose(dest);
                return ERR_EXIT;
            }
            
            std::fclose(dest);

            if (get_sha256(dest_path) != package.sha256) {
                return ERR_EXIT;
            }
        }
    }

    bool failed = false;
    for (size_t i = 0; i < plan.packages.size();) {
        InsPackage &package = plan.packages[i];
        if (failed) {
            if (!package.upgrade) {
                std::filesystem::path package_dir = root;
                package_dir /= PATHDIR_INSTALLED_PKGS;
                package_dir /= package.name;

                if (perform_remove(package_dir.c_str(), "package.files", root.c_str()) == ERR_OK) {
                    for (const auto &spec : package.dependencies) {
                        if (remove_dependent_reference(dependency_name(spec), package.name, root) != ERR_OK) {
                            return ERR_EXIT;
                        }
                    }
                }
            }

            if (i == 0) {
                break;
            }

            i--;

            continue;
        }

        int result;
        if (package.upgrade) {
            result = perform_upgrade(package, workdir, root);
        } else {
            result = perform_install(package, workdir, root);
        }

        if (result == ERR_RETRY && !package.upgrade) {
            result = perform_install(package, workdir, root);
        }

        if (result != ERR_OK) {
            failed = true;
            
            if (i == 0) {
                break;
            }

            i--;

            continue;
        }

        for (const auto &spec : package.dependencies) {
            if (add_dependent_reference(dependency_name(spec), package.name, root) != ERR_OK) {
                return ERR_EXIT;
            }
        }

        i++;
    }

    for (const auto &entry : std::filesystem::directory_iterator(workdir)) {
        std::error_code ec;
        std::filesystem::remove_all(entry.path(), ec);
        if (ec) {
            return ERR_EXIT;
        }
    }

    if (failed) {
        return ERR_EXIT;
    }

    return ERR_OK;
}

int perform_install(InsPackage package, std::string workdir, std::string root) {
    std::filesystem::path spkpath(workdir);
    spkpath /= package.name;
    std::ifstream spk(spkpath, std::ios::binary);
    if (!spk) {
        return ERR_EXIT;
    }

    std::filesystem::path install_dir = root;
    install_dir /= PATHDIR_INSTALLED_PKGS;

    std::filesystem::path package_dir = install_dir / package.name;

    if (std::filesystem::exists(package_dir / "package.metadata")) return ERR_EXIT;

    std::filesystem::create_directories(package_dir);

        // WRITE PACKAGE INFO
    std::ofstream metadataf(package_dir / "package.metadata");
    if (!metadataf) {
        perform_remove(package_dir.c_str(), "package.files", root.c_str());
        return ERR_RETRY;
    }

    metadataf << "version=" << package.version << std::endl;

    metadataf << "dependencies=";
    for (const auto &dependency : package.dependencies) {
        metadataf << dependency << ",";
    }
    metadataf << std::endl;

    metadataf << "dependents=";
    for (const auto &dependent : package.dependents) {
        metadataf << dependent << ",";
    }
    metadataf << std::endl;

    metadataf << "reason=";
    if (package.is_dependency) {
        metadataf << "dependency";
    } else {
        metadataf << "explicit";
    }
    metadataf << std::endl;

    if (perform_extract(spkpath.c_str(), root.c_str(), (package_dir / "install.journal").c_str()) != 0) {
        // REMOVE PACKAGE
        // MAY PERFORM RETRY DEPENDING ON ERROR
        perform_remove(package_dir.c_str(), "install.journal", root.c_str());
        return ERR_RETRY;
    } else {
        // SET JOURNAL AS INSTALL LIST
        std::filesystem::rename(package_dir / "install.journal", package_dir / "package.files");
    }

    return ERR_OK;
}

int perform_upgrade(InsPackage &package, std::string workdir, std::string root) {
    RemPackage old;
    if (get_package_removaldata(package.name, root, &old) != ERR_OK) {
        return ERR_EXIT;
    }

    package.dependents = old.dependents;


    std::filesystem::path package_dir = root;
    package_dir /= PATHDIR_INSTALLED_PKGS;
    package_dir /= package.name;

    if (perform_remove(package_dir.c_str(), "package.files", root.c_str()) != ERR_OK) {
        return ERR_EXIT;
    }

    for (const auto &dependency : old.dependencies) {
        if (remove_dependent_reference(dependency, package.name, root) != ERR_OK) {
            return ERR_EXIT;
        }
    }

    return perform_install(package, workdir, root);
}