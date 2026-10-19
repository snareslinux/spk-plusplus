#include <filesystem>
#include <fstream>

#include "spk.h"
#include "package.h"

int get_metadata(const char *path, struct metadata *out) {
    std::filesystem::path spkpath = path;
    std::ifstream spk(spkpath);
    if (!spk) {
        return -1;
    }

    struct spk_header header;
    spk.read(reinterpret_cast<char *>(&header), sizeof(header));
    spk.seekg(header.metadata_offset);
    spk.read(reinterpret_cast<char *>(out), sizeof(struct metadata));

    return 0;
}