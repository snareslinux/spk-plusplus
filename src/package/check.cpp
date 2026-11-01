#include <cstdio>

#include "spk.h"
#include "package/package.h"

bool check_spk_f(FILE *spk) {
    struct spk_header header;
    if (fread(&header, 1, sizeof(header), spk) != sizeof(header)) {
        return false;
    }
    return header.magic == SPK_MAGIC;
}

bool check_spk_path(const char *path) {
    FILE *spk = fopen(path, "rb");
    if (!spk) {
        return false;
    }

    bool res = check_spk_f(spk);
    fclose(spk);
    return res;
}