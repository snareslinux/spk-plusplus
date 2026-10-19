#include <stdbool.h>
#include <stdio.h>

#include "spk.h"

#ifndef PACKAGE_H
#define PACKAGE_H

#ifdef __cplusplus
extern "C" {
#endif

bool check_spk_f(FILE *spk);
bool check_spk_path(const char *path);

int create_spk(const char *srcdir);
int extract_spk(const char *spkpath, const char *extract_root, const char *logpath);

int get_metadata(const char *path, struct metadata *out);

#ifdef __cplusplus
}
#endif

#endif