#include <stdio.h>
#include <stddef.h>

#ifndef TAR_H
#define TAR_H

#ifdef __cplusplus
extern "C" {
#endif

int write_archive(const char *outname, const char *dirpath);
int extract_archive(const void *archive_mem, size_t mem_len, const char *destpath, FILE *extract_log);

#ifdef __cplusplus
}
#endif

#endif