#include <stdbool.h>

#ifndef INSTALL_H
#define INSTALL_H

#ifdef __cplusplus
extern "C" {
#endif

int install_packages(int argc, char **argv, bool assume_yes);
int perform_install(const char *spkpath);

#ifdef __cplusplus
}
#endif

#endif