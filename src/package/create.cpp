#include <string>
#include <fstream>
#include <filesystem>
#include <cstring>
#include <iostream>

#include "ui/ui.hpp"
#include "tar/tar.h"
#include "util/util.h"
#include "spk.h"
#include "package.h"

void copy_value(char *dest, size_t size, const std::string &str) {
    std::string value = str.substr(str.find('=') + 1);
    if (value.size() >= size) {
        return;
    }
    
    std::memcpy(dest, value.c_str(), value.size() + 1);
}

int create_spk(const char *srcdir) {
    std::filesystem::path src = srcdir;

    struct spk_header header {};
    struct metadata metadata {};

    std::ifstream metadata_f(src / "metadata");
    if (!metadata_f) {
        return 1;
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
        if (entry.compare(0, 8, "depends=") == 0) {
            copy_value(metadata.dependencies, sizeof(metadata.dependencies), entry);
        }
    }

    int archiveui = ui.create_task("Creating files archive");

    auto archive = std::filesystem::temp_directory_path() / "spk-files.tar.zst";
    
    if (!std::filesystem::exists(src / "files")) {
        return 1;
    } else {
        if (write_archive(archive.c_str(), (src / "files").c_str()) != 0) {
            return 1;
        }
    }

    std::ifstream files_f(archive.c_str(), std::ios::binary);
    if (!files_f) {
        return 1;
    }

    std::ofstream spk_f(std::string(metadata.name) + "-" + std::string(metadata.version) + ".spk", std::ios::binary);
    if (!spk_f) {
        return 1;
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

    return 0;
}