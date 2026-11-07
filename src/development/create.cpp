#include <string>
#include <fstream>
#include <filesystem>
#include <cstring>
#include <iostream>

#include "err.h"
#include "ui/ui.hpp"
#include "tar/tar.h"
#include "util/util.h"
#include "package/package.h"
#include "spk.h"

void copy_value(char *dest, size_t size, const std::string &str) {
    std::string value = str.substr(str.find('=') + 1);
    if (value.size() >= size) {
        return;
    }
    
    std::memcpy(dest, value.c_str(), value.size() + 1);
}

int write_spk(const char *srcdir, const char *out) {
    std::filesystem::path src = srcdir;

    struct spk_header header {};
    struct metadata metadata {};

    std::ifstream metadata_f(src / "metadata");
    if (!metadata_f) {
        return ERR_EXIT;
    }
    
    std::string entry;
    while (std::getline(metadata_f, entry, ';')) {
        entry.erase(0, entry.find_first_not_of(" \t\r\n"));
        if (entry.compare(0, 5, "name=") == 0) {
            copy_value(metadata.name, sizeof(metadata.name), entry);
        }
        if (entry.compare(0, 8, "version=") == 0) {
            copy_value(metadata.version, sizeof(metadata.version), entry);
        }
        if (entry.compare(0, 13, "architecture=") == 0) {
            copy_value(metadata.arch, sizeof(metadata.arch), entry);
        }
        if (entry.compare(0, 13, "dependencies=") == 0) {
            copy_value(metadata.dependencies, sizeof(metadata.dependencies), entry);
        }
    }

    int archiveui = ui.create_task("Creating files archive");

    auto archive = std::filesystem::temp_directory_path() / "spk-files.tar.zst";
    
    if (!std::filesystem::exists(src / "files")) {
        return ERR_EXIT;
    } else {
        if (write_archive(archive.c_str(), (src / "files").c_str()) != 0) {
            return ERR_EXIT;
        }
    }

    std::ifstream files_f(archive.c_str(), std::ios::binary);
    if (!files_f) {
        return ERR_EXIT;
    }
    
    std::string output_name;
    if (out == 0) {
        output_name = std::string(metadata.name) + "-" + std::string(metadata.version) + ".spk";
    } else {
        output_name = out;
        if (output_name.empty()) {
            return ERR_EXIT;
        }

        if (!output_name.contains(".spk")) {
            output_name += ".spk";
        }
    }

    std::ofstream spk_f(output_name, std::ios::binary);
    if (!spk_f) {
        return ERR_EXIT;
    }

    ui.end_task(archiveui, "Package files compressed");

    int writingui = ui.create_task("Writing SPK");

    header.magic = SPK_MAGIC;
    header.metadata_offset = sizeof(struct spk_header);
    header.metadata_size = sizeof(struct metadata);
    header.data_offset = header.metadata_offset + header.metadata_size;

    files_f.seekg(0, std::ios::end);
    std::streamsize size = files_f.tellg();
    files_f.seekg(0);
    header.data_size = size;

    spk_f.seekp(header.data_offset, std::ios::beg);
    spk_f << files_f.rdbuf();
    spk_f.seekp(header.metadata_offset, std::ios::beg);
    spk_f.write(reinterpret_cast<const char *>(&metadata), sizeof(metadata));
    spk_f.seekp(0, std::ios::beg);
    spk_f.write(reinterpret_cast<const char *>(&header), sizeof(header));

    ui.end_task(writingui, "SPK written");

    std::filesystem::remove(archive);

    return ERR_OK;
}