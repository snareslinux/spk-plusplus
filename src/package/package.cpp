#include <filesystem>
#include <fstream>

#include "err.h"
#include "spk.h"
#include "package/package.h"

int get_metadata(const char *path, struct metadata *out) {
    std::filesystem::path spkpath = path;

    std::ifstream spk(spkpath, std::ios::binary);
    if (!spk) {
        return ERR_EXIT;
    }

    struct spk_header header {};
    spk.read(reinterpret_cast<char *>(&header), sizeof(header));

    if (!spk || header.magic != SPK_MAGIC || header.metadata_size != sizeof(struct metadata)) {
        return ERR_EXIT;
    }

    spk.seekg(header.metadata_offset, std::ios::beg);

    if (!spk) {
        return ERR_EXIT;
    }

    spk.read(reinterpret_cast<char *>(out), sizeof(struct metadata));
    if (!spk) {
        return ERR_EXIT;
    }

    out->name[sizeof(out->name) - 1] = '\0';
    out->version[sizeof(out->version) - 1] = '\0';
    out->arch[sizeof(out->arch) - 1] = '\0';
    out->dependencies[sizeof(out->dependencies) - 1] = '\0';

    return ERR_OK;
}