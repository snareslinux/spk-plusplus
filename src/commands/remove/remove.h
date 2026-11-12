#include <stdbool.h>

#ifndef REMOVE_H
#define REMOVE_H

#ifdef __cplusplus
extern "C" {
#endif

int remove_packages(int argc, char **argv, bool assume_yes, bool force, const char *root);
int perform_remove(const char *pkgdir, const char *target, const char *root);

#ifdef __cplusplus
}
#endif

#endif