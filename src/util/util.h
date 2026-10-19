#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>

#ifndef UTIL_H
#define UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

size_t copy_file(FILE *src, FILE *dest, uint64_t total);

int mkdir_p(const char *dir, mode_t mode);
void clean_string(char *buffer);
int download_to_file(const char *url, FILE *file);

#ifdef __cplusplus
}
#endif

#endif