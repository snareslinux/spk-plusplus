#include <fstream>
#include <filesystem>
#include <vector>

#include "ui/ui.hpp"
#include "util/util.h"
#include "spk.h"
#include "tar/tar.h"
#include "package.h"

int extract_spk(const char *spkpath, const char *extract_root, const char *logpath) {
    if (!check_spk_path(spkpath)) {
        return 1;
    }
    
    std::ifstream spk_f(spkpath);
    if (!spk_f) {
        return 1;
    }
    
    struct spk_header header;
    spk_f.read(reinterpret_cast<char *>(&header), sizeof(header));

    spk_f.seekg(header.data_offset, std::ios::beg);
    std::vector<char> buffer(header.data_size);
    spk_f.read(buffer.data(), header.data_size);

    int extractui = ui.create_task("Extracting files to \"%s\"", extract_root);

    if (logpath) {
        FILE *log = fopen(logpath, "w");
        if (!log) {
            return 1;
        }

        if (extract_archive(buffer.data(), header.data_size, extract_root, log) != 0) {
            fclose(log);
            return 1;
        }

        fclose(log);
    } else {
        if (extract_archive(buffer.data(), header.data_size, extract_root, NULL) != 0) {
            return 1;
        }
    }

    ui.end_task(extractui, "Extraction Done!");

    return 0;
}