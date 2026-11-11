#include <fstream>
#include <filesystem>
#include <vector>

#include "err.h"
#include "ui/ui.hpp"
#include "util/util.h"
#include "tar/tar.h"
#include "package/package.h"
#include "spk.h"

int perform_extract(const char *spkpath, const char *extract_root, const char *logpath) {
    if (!check_spk_path(spkpath)) {
        return ERR_EXIT;
    }
    
    std::ifstream spk_f(spkpath);
    if (!spk_f) {
        return ERR_EXIT;
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
            return ERR_EXIT;
        }

        if (extract_archive(buffer.data(), header.data_size, extract_root, log) != 0) {
            fclose(log);
            return ERR_EXIT;
        }

        fclose(log);
    } else {
        if (extract_archive(buffer.data(), header.data_size, extract_root, NULL) != 0) {
            return ERR_EXIT;
        }
    }

    ui.end_task(extractui, "Extraction Done!");

    return ERR_OK;
}