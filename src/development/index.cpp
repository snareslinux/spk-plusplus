#include <fstream>
#include <filesystem>
#include <vector>
#include <string>

#include "err.h"
#include "spk.h"
#include "util/util.hpp"
#include "commands/commands.h"
#include "package/package.h"

int write_index(const char *repository) {
    std::filesystem::path repo = repository;
    std::filesystem::path packages = repo / "packages";

    if (!std::filesystem::is_directory(packages)) {
        return ERR_EXIT;
    }

    std::ofstream index(repo / "index", std::ios::trunc);
    if (!index) {
        return ERR_EXIT;
    }

    for (const auto &entry : std::filesystem::directory_iterator(packages)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".spk") {
            continue;
        }

        struct metadata metadata {};

        if (!check_spk_path(entry.path().c_str())) {
            return ERR_EXIT;
        }

        if (get_metadata(entry.path().c_str(), &metadata) != ERR_OK) {
            return ERR_EXIT;
        }



        index << metadata.name << "|"
            << metadata.version << "|"
            << metadata.arch << "|"
            << entry.path().filename().string() << "|"
            << std::filesystem::file_size(entry.path()) << "|"
            << get_sha256(entry.path()) << "|"
            << metadata.dependencies << std::endl;
    }

    return ERR_OK;
}